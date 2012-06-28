/*
 *
 @file		serial.c
 @brief	UART functions for AVR Evaluation Board  (AVR-GCC Compiler)
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h> 
#include <stdlib.h>

#include "types.h"
#include "serial.h"

typedef struct _UARTHANDLER {
  char (*uart_get_handler)(FILE *);
  void (*uart_put_handler)(char, FILE *);
} UARTHANDLER;

#define SET_MY_XOFF_STATE(X)	(bXONOFF[X] |= 0x01)
#define SET_MY_XON_STATE(X)	(bXONOFF[X] &= ~(0x01))
#define SET_PEER_XOFF_STATE(X)	(bXONOFF[X] |= 0x02)
#define SET_PEER_XON_STATE(X)	(bXONOFF[X] &= ~(0x02))

static u_char sio_rxd[UART_DEVICE_CNT][MAX_SIO_COUNT];
//static int sio_head[UART_DEVICE_CNT];				/**< a pointer to serial Rx buffer */
//static int sio_tail[UART_DEVICE_CNT];				/**< a pointer to serial Rx buffer */

volatile static int sio_head[UART_DEVICE_CNT]; /**< a pointer to serial Rx buffer *////+2007113[jhpark] Optimize
volatile static int sio_tail[UART_DEVICE_CNT]; /**< a pointer to serial Rx buffer */

static u_char SIO_FLOW[UART_DEVICE_CNT];
static u_char bXONOFF[UART_DEVICE_CNT]; /**< XON/XOFF flag bit : 0 bit(1-MY XOFF State, 0-MY XON State), 1 bit(1 - Peer XOFF, 0-Peer XON) */

static UARTHANDLER uart_handler[UART_DEVICE_CNT];

extern char uart0_getchar(FILE *f);
extern void uart0_putchar(char c, FILE *f);


extern char uart1_getchar(FILE *f);
extern void uart1_putchar(char c, FILE *f);

/**
 @brief	ATmega128 UART Rx ISR

 This function is the signal handler for receive complete interrupt.
 for UART0 Internal Function

 */
ISR(USART0_RX_vect)
/* Interrupt Service Routine for AVR GCC ver. 3.4.6*/
{
  sio_rxd[0][sio_head[0]] = UDR0; /* read RX data from UART0 */

  if (SIO_FLOW[0]) {
    if (sio_rxd[0][sio_head[0]] == XOFF_CHAR)
      SET_PEER_XOFF_STATE(0);
    else if (sio_rxd[0][sio_head[0]] == XON_CHAR)
      SET_PEER_XON_STATE(0);
    else
      sio_head[0]++;
  } else
    sio_head[0]++;

  if (sio_head[0] == sio_tail[0]) {
    if (SIO_FLOW[0]) {
      while (!(UCSR0A & 0x20))
        ;
      UDR0 = XOFF_CHAR;
      SET_MY_XOFF_STATE(0);
    }
    sio_head[0]--; /* buffer full. */
  }
  if (sio_head[0] >= MAX_SIO_COUNT) /* for ring buffer */
  {
    sio_head[0] = 0;
    if (sio_head[0] == sio_tail[0])
      sio_head[0] = MAX_SIO_COUNT;
  }
}

#ifdef SUPPORT_UART_ONE	/* UART1 */

/**
 @brief	ATmega128 UART Rx ISR

 This function is the signal handler for receive complete interrupt.
 for UART1 Internal Function

 */
ISR(USART1_RX_vect) /* Interrupt Service Routine for AVR GCC ver. 3.4.6*/
{
  sio_rxd[1][sio_head[1]] = UDR1; /* read RX data from UART0 */

  if(SIO_FLOW[1])
  {
    if(sio_rxd[1][sio_head[1]] == XOFF_CHAR)
    SET_PEER_XOFF_STATE(1);
    else if(sio_rxd[1][sio_head[1]] == XON_CHAR)
    SET_PEER_XON_STATE(1);
    else sio_head[1]++;
  }
  else sio_head[1]++;

  if (sio_head[1] == sio_tail[1])
  {
    if(SIO_FLOW[1])
    {
      while (!(UCSR1A & 0x20));
      UDR1 = XOFF_CHAR;
      SET_MY_XOFF_STATE(1);
    }
    sio_head[1]--; /* buffer full. */
  }
  if (sio_head[1] >= MAX_SIO_COUNT) /* for ring buffer */
  {
    sio_head[1] = 0;
    if (sio_head[1] == sio_tail[1]) sio_head[1] = MAX_SIO_COUNT;
  }
}

#endif

void setBaudRate(uint32 baudRate,uint8 uartNr) {
  uint16 u16UBRR;
  ldiv_t xDiv;
  // Calculate new 16-bit UBBR register value
  baudRate <<= 4;
  xDiv = ldiv(F_CPU, baudRate);
  u16UBRR = (uint16) xDiv.quot;
  baudRate >>= 1;
  if ((uint32)(xDiv.rem) < baudRate) {
    u16UBRR--;
  }
  switch (uartNr) {
    case 0:
      UBRR0H = (u_char) ((u16UBRR >> 8) & 0xFF);
      UBRR0L = ((u_char) (u16UBRR & 0xFF));
      break;
#ifdef UBRR1
      case 1:
      UBRR1 = u16UBRR;
      break;
#endif
#ifdef UBRR2
      case 2:
      UBRR2 = u16UBRR;
      break;
#endif
#ifdef UBRR3
      case 3:
      UBRR3 = u16UBRR;
      break;
#endif
    default:
      break;
  }
}

/**
 @brief	This function initializes the UART of ATmega64
 */
void uart_init(u_char uart, /**< UART Device Index(0,1) */
u_char baud_index /**< UART BaudRate Index(0:2400,...,11:500000) */
) {

  /* enable RxD/TxD and RX INT */
  u_int uart_select_baud = UART_BAUD_SELECT(baud_index);
  if (uart == 0) /* uart == 0( first serial ) */
  {
    PORTE |= 3;
    DDRE &= ~(1);
    DDRE |= (2);

    UCSR0B = (1 << RXCIE) | (1 << RXEN) | (1 << TXEN);

    //setBaudRate(2400,0);
    UBRR0H = (u_char) ((uart_select_baud >> 8) & 0xFF);
    UBRR0L = (u_char) (uart_select_baud & 0xFF);

    uart_handler[0].uart_get_handler = uart0_getchar;
    uart_handler[0].uart_put_handler = uart0_putchar;

    /* New prototype for avr-libc 1.4 and above. */

    /* fdevopen((void *)uart0_putchar_avrlibv1_4, (void *)uart0_getchar_avrlibv1_4); */
    fdevopen((void *) uart0_putchar, (void *) uart0_getchar);

  }
#ifdef SUPPORT_UART_ONE	
  else if ( uart == 1 )
  {
    UCSR1B = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);
    /* set baud rate */
    UBRR1H = (u_char) ((uart_select_baud >> 8) & 0xFF);
    UBRR1L = (u_char) (uart_select_baud& 0xFF);

    uart_handler[1].uart_get_handler = uart1_getchar;
    uart_handler[1].uart_put_handler = uart1_putchar;
    /* New prototype for avr-libc 1.4 and above. */

    /* fdevopen((void *)uart0_putchar_avrlibv1_4, (void *)uart0_getchar_avrlibv1_4); */
    fdevopen((void *)uart1_putchar, (void *)uart1_getchar);
  }
#endif	
  else {
    return;
  }
  sio_head[uart] = 0;
  sio_tail[uart] = 0;

  SIO_FLOW[uart] = 0;
  bXONOFF[uart] = 0;
}
////---- END_MODIFY


void uart_databit(u_char uart, u_char dbit) {
  if (uart == 0) {
    UCSR0C |= 1 << 3;
    if (!dbit)
      UCSR0C &= ~(1 << 1); // 7bit
    else
      UCSR0C |= (1 << 1); // 8bit;
  }
#ifdef SUPPORT_UART_ONE		
  else if ( uart == 1 )
  {
    UCSR1C |= 1 << 3;
    if(!dbit) UCSR1C &= ~(1<<1); // 7bit
    else UCSR1C |= (1<<1); // 8bit;
  }
#endif	
}

void uart_stopbit(u_char uart, u_char sbit) {
  if (uart == 0) {
    if (!sbit)
      UCSR0C &= ~(1 << 3); // 1 BIT
    else
      UCSR0C |= (1 << 3); // 2 BIT
  }
#ifdef SUPPORT_UART_ONE	
  else if ( uart == 1 )
  {
    if(!sbit) UCSR1C &= ~(1 << 3); // 1 BIT
    else UCSR1C |= (1 << 3); // 2 BIT}
  }
#endif	
}

void uart_paritybit(u_char uart, u_char pbit) {
  if (uart == 0) {
    if (!pbit)
      UCSR0C &= ~(3 << 4); // none
    else if (pbit == 1)
      UCSR0C &= ~(1 << 4); // even
    else
      UCSR0C |= (3 << 4); // odd
  }
#ifdef SUPPORT_UART_ONE	
  else if ( uart == 1 )
  {
    if(!pbit) UCSR1C &= ~(3 << 4); // none
    else if (pbit ==1) UCSR1C &= ~(1 << 4); // even
    else UCSR1C |= (3 << 4); // odd
  }
#endif	
}

void uart_flowctrl(u_char uart, u_char flow) {
  if (uart < UART_DEVICE_CNT)
    SIO_FLOW[uart] = flow;
}

/**
 @brief	CHECK RX
 @return	retunr RX size, if there is Rx.

 This function checks if there is Rx. \n
 if not, return 0

 */

u_int uart_keyhit(u_char uart) {
  if (uart >= UART_DEVICE_CNT)
    return 0;

  if (sio_head[uart] >= sio_tail[uart])
    return (sio_head[uart] - sio_tail[uart]);
  else
    return (MAX_SIO_COUNT - (sio_tail[uart] - sio_head[uart]));
}

/**
 @brief	WRITE A CHARACTER

 This function sends a character through UART0.

 */
void uart0_putchar(char c, FILE *f) {
  while (SIO_FLOW[0] && (bXONOFF[0] & 0x02))
    ; // If Peer XOFF STATE
  while (!(UCSR0A & 0x20))
    ;
  UDR0 = c;
}

/**
 @brief	READ A CHARACTER
 @return	c - is a character to read

 This function gets a character from UART0.
 */
char uart0_getchar(FILE *f) {
  char c;
  while (sio_head[0] == sio_tail[0])
    ;

  c = sio_rxd[0][sio_tail[0]++];

  if (SIO_FLOW[0] && (bXONOFF[0] & 0x01)) // IF MY XOFF STATE
  {
    while (!(UCSR0A & 0x20))
      ;
    UDR0 = XON_CHAR;
    SET_MY_XON_STATE(0);
  }

  if (sio_tail[0] >= MAX_SIO_COUNT)
    sio_tail[0] = 0;

  return c;
}

#ifdef SUPPORT_UART_ONE
/**
 @brief	WRITE A CHARACTER

 This function sends a character through UART1.

 */
void uart1_putchar(char c, FILE *f)
{
  while(SIO_FLOW[1] && (bXONOFF[1] & 0x02)); // If Peer XOFF STATE
  while (!(UCSR1A & 0x20));
  UDR1 = c;
}

/**
 @brief	READ A CHARACTER
 @return	c - is a character to read

 This function gets a character from UART1.

 */
char uart1_getchar(FILE *f)
{
  char c;

  while (sio_head[1] == sio_tail[1]);

  c = sio_rxd[1][sio_tail[1]++];

  if(SIO_FLOW[1] && (bXONOFF[1] & 0x01)) // IF MY XOFF STATE
  {
    while (!(UCSR1A & 0x20));
    UDR1 = XON_CHAR;
    SET_MY_XON_STATE(1);
  }

  if (sio_tail[1] >= MAX_SIO_COUNT) sio_tail[1] = 0;

  return c;
}

#endif	/* support UART1 */

/**
 @brief	WRITE A CHARACTER

 This function sends a string to UART.

 */
void uart_puts(u_char uart, char * str) {
  int i = 0;

  if (uart >= UART_DEVICE_CNT)
    return;

  while (str[i])
    (*uart_handler[uart].uart_put_handler)(str[i++], NULL);
}

/**
 @brief	READ A CHARACTER
 @return	str - is a pointer to the string to read

 This function gets a string from UART.

 */
int uart_gets(u_char uart, char * str, char bpasswordtype, int max_len) {
  char c;
  char * tsrc = str;
  char IsFirst = 1;
  int len = 0;

  if (uart >= UART_DEVICE_CNT)
    return 0;

  while ((c = (*uart_handler[uart].uart_get_handler)(NULL)) != 0x0D) {
    if (IsFirst && c == '!') {
      while (*str != '\0')
        (*uart_handler[uart].uart_put_handler)(*str++, NULL);
      IsFirst = 0;
      len++;
      continue;
    }
    if (c == 0x08 && tsrc != str) {
      (*uart_handler[uart].uart_put_handler)(0x08, NULL);
      (*uart_handler[uart].uart_put_handler)(' ', NULL);
      (*uart_handler[uart].uart_put_handler)(0x08, NULL);
      str--;
      len--;
      continue;
    } else if (c == 0x1B) {
      while (tsrc != str) {
        (*uart_handler[uart].uart_put_handler)(0x08, NULL);
        (*uart_handler[uart].uart_put_handler)(' ', NULL);
        (*uart_handler[uart].uart_put_handler)(0x08, NULL);
        str--;
        len--;
      }
      IsFirst = 1;
      continue;
    } else if ((c < 32 || c > 126) && c != '\t')
      continue;
    if (len < max_len) {
      if (bpasswordtype)
        (*uart_handler[uart].uart_put_handler)('*', NULL);
      else
        (*uart_handler[uart].uart_put_handler)(c, NULL);
      *str++ = c;
      len++;
      IsFirst = 0;
    }
  }
  *str = '\0';
  uart_puts(uart, "\r\n");

  return len;
}

/**
 @brief	GET A BYTE

 This function flush rx buffer of serial

 */
void uart_flush_rx(u_char uart) {
  if (uart >= UART_DEVICE_CNT)
    return;

  sio_head[uart] = sio_tail[uart];
}

