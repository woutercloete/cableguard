#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support
/****************************************************************************************/
#include "avrlibtypes.h"
#include "avrlibdefs.h"
#include "CUART.h"
#include "iopins.h"
#include "common.h"



/****************************************************************************************/
#ifndef UDR0
#define UCSR0A UCSRA
#define UCSR0B UCSRB
#define UCSR0C UCSRC
#define UCSZ00 UCSZ0
#define UCSZ01 UCSZ1
#define UCSZ02 UCSZ2
#define USBS0 USBS
#define UBRR0H UBRRH
#define UBRR0L UBRRL
#define UDR0 UDR
#define UDRE0 UDRE
#define UDRIE0 UDRIE
#define RXCIE0 RXCIE
#define TXCIE0 TXCIE
#define RXEN0 RXEN
#define TXEN0 TXEN
#define TXC0 TXC
#define RXC0 RXC
#define UPM00 UPM0
#define UPM01 UPM1
#endif
/****************************************************************************************/
#define UART_INACTIVE_TIME    1000  //us
/****************************************************************************************/
static CUART* pUart[4];
/****************************************************************************************/
CUART::CUART(u08 uartNr, u32 baudRate, u16 bufSize) {
	this->uartNr = uartNr;
	this->baudRate = baudRate;
	healthy = true;
	if (!rxFIFO.setBufSize(bufSize))
		healthy = false;
	if (!txFIFO.setBufSize(bufSize))
		healthy = false;
	this->enable485 = false;
	// enable RxD/TxD and interrupts
	switch (uartNr) {
	case 0:
		UCSR0B = BV(RXCIE0) | BV(TXCIE0) | BV(RXEN0) | BV(TXEN0);
		pUart[0] = this;
		break;
#ifdef UCSR1B
	case 1:
		UCSR1B = BV(RXCIE1) | BV(TXCIE1) | BV(RXEN1) | BV(TXEN1);
		pUart[1] = this;
		break;
#endif
#ifdef UCSR2B
	case 2:
		UCSR2B = BV(RXCIE2) | BV(TXCIE2) | BV(RXEN2) | BV(TXEN2);
		pUart[2] = this;
		break;
#endif
#ifdef UCSR3B
	case 3:
		UCSR3B = BV(RXCIE3) | BV(TXCIE3) | BV(RXEN3) | BV(TXEN3);
		pUart[3] = this;
		break;
#endif
	}
	setBaudRate(baudRate);
	rxOverflowCnt = 0;
	txBusy = false;
}

/****************************************************************************************/
CUART::CUART(u08 uartNr, u32 baudRate, u16 bufSize, u08 enable485) {
	this->uartNr = uartNr;
	healthy = true;
	if (!rxFIFO.setBufSize(bufSize))
		healthy = false;
	if (!txFIFO.setBufSize(bufSize))
		healthy = false;
	this->enable485 = true;
	// enable RxD/TxD and interrupts
	switch (uartNr) {
	case 0:
		UCSR0B = BV(RXCIE0) | BV(TXCIE0) | BV(RXEN0) | BV(TXEN0);
		pUart[0] = this;
		break;
#ifdef UCSR1B
	case 1:
		UCSR1B = BV(RXCIE1) | BV(TXCIE1) | BV(RXEN1) | BV(TXEN1);
		pUart[1] = this;
		break;
#endif
#ifdef UCSR2B
	case 2:
	  UCSR2B = BV(RXCIE2) | BV(TXCIE2) | BV(RXEN2) | BV(TXEN2);
		pUart[2] = this;
		break;
#endif
#ifdef UCSR3B
	case 3:
	  UCSR3B = BV(RXCIE3) | BV(TXCIE3) | BV(RXEN3) | BV(TXEN3);
		pUart[3] = this;
		break;
#endif
	}
	setBaudRate(baudRate);
	rxOverflowCnt = 0;
	txBusy = false;
	BIT_SET_HI(RS485_EN_DDR, RS485_EN_PIN);
	BIT_SET_LO(RS485_EN_PORT, RS485_EN_PIN);
	//setFrame();
}
/****************************************************************************************/
void CUART::clearRx(void) {
	rxFIFO.clear();
}
/****************************************************************************************/
// prints a null-terminated string stored in program ROM
#ifndef UART_MINIMAL
void CUART::sendStr_P(const prog_char str[])
{
  register char c;
  if (!str) return;
  // print the string until the null-terminator
  while((c = pgm_read_byte(str++)))
    send(&c,1);
}

void CUART::sendStr(c08* str)
{
  send(str,strlen(str));
}

u16 CUART::send_P(const prog_char buf[], u16 nBytes) {
  u16 res;
  u08 dat;
  if (!nBytes || !buf) {
    return 0;
  }
  while(nBytes){
    dat = pgm_read_byte(buf++);
    res = txFIFO.add(&dat,1);
    nBytes--;
  }
  return res;
}

void CUART::uprintf(const char *__fmt, ...) {
  c08 str[255];
  va_list arg;
  va_start(arg,__fmt);
  vsprintf(str,__fmt,arg);
  va_end(arg);
  send(str,strlen(str));
}

#endif
/****************************************************************************************/
u16 CUART::send(c08* buffer, u16 nBytes) {
	u16 res;
	if (!nBytes || !buffer) {
		return 0;
	}
	res = txFIFO.add((u08 *)buffer, nBytes);
	switch (uartNr) {
	case 0:
		// enable UDRE interrupt => we will start sending as soon as interrupt is handled
		UCSR0B |= BV(UDRIE0);
		break;
#ifdef UDR1
	case 1:
		UCSR1B |= BV(UDRIE1);
		break;
#endif
#ifdef UDR2
	case 2:
		UCSR2B |= BV(UDRIE2);
		break;
#endif
#ifdef UDR2
	case 3:
		UCSR3B |= BV(UDRIE3);
		break;
#endif
	}
	//return number of bytes written
	return res;
}
/****************************************************************************************/
u16 CUART::receive(u08* buffer, u16 nBytes) {
	return rxFIFO.remove(buffer, nBytes);
}

u16 CUART::peek(c08* buffer) {
  return rxFIFO.peek(buffer);
}

u16 CUART::space(void) {
	return txFIFO.space();
}

u16 CUART::rxnum(void) {
  return rxFIFO.received();
}


/****************************************************************************************/
void CUART::setBaudRate(u32 baudRate) {
	u16 u16UBRR;
	ldiv_t xDiv;
	this->baudRate = baudRate;

	// Calculate new 16-bit UBBR register value
	baudRate <<= 4;
	xDiv = ldiv(F_CPU, baudRate);
	u16UBRR = (u16) xDiv.quot;
	baudRate >>= 1;
	if ((u32) (xDiv.rem) < baudRate) {
		u16UBRR--;
	}
	switch (uartNr) {
	// Set BAUD rate by initalising 16-bit UBBR register
	case 0:
#if defined(__AVR_ATmega8__)
		// Set baud rate
		UCSRC = 0;
		UBRRH = U16_HI(u16UBRR);
		UBRRL = U16_LO(u16UBRR);
#else
    UBRR0H = U16_HI(u16UBRR);
    UBRR0L = U16_LO(u16UBRR);
#endif
		break;
	case 1:
    UBRR1H = U16_HI(u16UBRR);
    UBRR1L = U16_LO(u16UBRR);
		break;
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
/****************************************************************************************/
void CUART::setFrame(void) {
	// Set frame format to 8 data bits, 1 stop bit and no parity
	switch (uartNr) {
	case 0:
		sbi(UCSR0C,UCSZ00);
		sbi(UCSR0C,UCSZ01);
		cbi(UCSR0C,UCSZ02);
		cbi(UCSR0C,USBS0);
		cbi(UCSR0C,UPM00);
		cbi(UCSR0C,UPM01);
		break;
#ifdef UCSR1C
	case 1:
		sbi(UCSR1C,UCSZ10);
		sbi(UCSR1C,UCSZ11);
		cbi(UCSR1C,UCSZ12);
		cbi(UCSR1C,USBS1);
		cbi(UCSR1C,UPM10);
		cbi(UCSR1C,UPM11);
		break;
#endif
#ifdef UCSR2C
	case 2:
		sbi(UCSR2C,UCSZ20);
		sbi(UCSR2C,UCSZ21);
		cbi(UCSR2C,UCSZ22);
		cbi(UCSR2C,USBS2);
		cbi(UCSR2C,UPM20);
		cbi(UCSR2C,UPM21);
		break;
#endif
#ifdef UCSR3C
	case 3:
		sbi(UCSR3C,UCSZ30);
		sbi(UCSR3C,UCSZ31);
		cbi(UCSR3C,UCSZ32);
		cbi(UCSR3C,USBS3);
		cbi(UCSR3C,UPM30);
		cbi(UCSR3C,UPM31);
		break;

#endif
	}
}
//===============================================================================
// UART Transmit register empty
//===============================================================================
void SIG_UART_DATA(void) {
	u08 dat;
	u08 cnt;
	if (pUart[0]->enable485 == true) {
		BIT_SET_HI(RS485_EN_PORT, RS485_EN_PIN);
	}
	cnt = pUart[0]->txFIFO.remove(&dat, 1);
	if (cnt) {
		UDR0 = dat;
	} else {
		UCSR0B &= ~(BV(UDRIE0)); /* disable UDRE interrupt */
	}
}
//===============================================================================
// UART Transmit Complete Interrupt Handler
//===============================================================================
void SIG_UART_TRANS(void) {
	if (pUart[0]->enable485 == true) {
		BIT_SET_LO(RS485_EN_PORT, RS485_EN_PIN);
	}
}
//===============================================================================
// UART Receive Complete Interrupt Handler
//===============================================================================
void SIG_UART_RECV(void) {
	u08 data;
	/* read the received data */
	data = UDR0;
	pUart[0]->rxFIFO.add(&data, 1);
}
//===============================================================================
// UART Transmit register empty
//===============================================================================
void SIG_USART0_DATA(void) {
	u08 dat;
	u08 cnt;
	if (pUart[0]->enable485 == true) {
		BIT_SET_HI(RS485_EN_PORT, RS485_EN_PIN);
	}
	cnt = pUart[0]->txFIFO.remove(&dat, 1);
	if (cnt) {
		UDR0 = dat;
	} else {
		UCSR0B &= ~(BV(UDRIE0)); /* disable UDRE interrupt */
	}
}
//===============================================================================
// UART Transmit Complete Interrupt Handler
//===============================================================================
void SIG_USART0_TRANS(void) {
	if (pUart[0]->enable485 == true) {
		BIT_SET_LO(RS485_EN_PORT, RS485_EN_PIN);
	}
}
//===============================================================================
// UART Receive Complete Interrupt Handler
//===============================================================================
void SIG_USART0_RECV(void) {
	u08 data;
	/* read the received data */
	data = UDR0;
	pUart[0]->rxFIFO.add(&data, 1);
}

//===============================================================================
// UART1 Transmit register empty
//===============================================================================
#ifdef UDR1
void SIG_USART1_DATA(void) {
	u08 dat;
	u08 cnt;
	if (pUart[1]->enable485 == true) {
		BIT_SET_HI(RS485_EN_PORT, RS485_EN_PIN);
	}
	cnt = pUart[1]->txFIFO.remove(&dat, 1);
	if (cnt) {
		UDR1 = dat;
	} else {
		UCSR1B &= ~(BV(UDRIE1)); /* disable UDRE interrupt */
	}
}
//===============================================================================
// UART Transmit Complete Interrupt Handler
//===============================================================================
void SIG_USART1_TRANS(void) {
	if (pUart[1]->enable485 == true) {
		BIT_SET_LO(RS485_EN_PORT, RS485_EN_PIN);
	}
}
//===============================================================================
// UART Receive Complete Interrupt Handler
//===============================================================================
void SIG_USART1_RECV(void) {
	u08 data;
	/* read the received data */
	data = UDR1;
	pUart[1]->rxFIFO.add(&data, 1);
}
#endif
#ifdef UDR2
//===============================================================================
// UART2 Transmit register empty
//===============================================================================
void SIG_USART2_DATA(void) {
	u08 dat;
	u08 cnt;
	if (pUart[2]->enable485 == true) {
		BIT_SET_HI(RS485_EN_PORT, RS485_EN_PIN);
	}
	cnt = pUart[2]->txFIFO.remove(&dat, 1);
	if (cnt) {
		UDR2 = dat;
	} else {
		UCSR2B &= ~(BV(UDRIE2)); /* disable UDRE interrupt */
	}
}
//===============================================================================
// UART Transmit Complete Interrupt Handler
//===============================================================================
void SIG_USART2_TRANS(void) {
	if (pUart[2]->enable485 == true) {
		BIT_SET_LO(RS485_EN_PORT, RS485_EN_PIN);
	}
}
//===============================================================================
// UART Receive Complete Interrupt Handler
//===============================================================================
void SIG_USART2_RECV(void) {
	u08 data;
	/* read the received data */
	data = UDR2;
	pUart[2]->rxFIFO.add(&data, 1);
}
#endif

//===============================================================================
// UART Transmit register empty
//===============================================================================
#ifdef UDR3
void SIG_USART3_DATA(void) {
	u08 dat;
	u08 cnt;
	if (pUart[3]->enable485 == true) {
		BIT_SET_HI(RS485_EN_PORT, RS485_EN_PIN);
	}
	cnt = pUart[3]->txFIFO.remove(&dat, 1);
	if (cnt) {
		UDR3 = dat;
	} else {
		UCSR3B &= ~(BV(UDRIE3)); /* disable UDRE interrupt */
	}
}
//===============================================================================
// UART Transmit Complete Interrupt Handler
//===============================================================================
void SIG_USART3_TRANS(void) {
	if (pUart[3]->enable485 == true) {
		BIT_SET_LO(RS485_EN_PORT, RS485_EN_PIN);
	}
}

//===============================================================================
// UART Receive Complete Interrupt Handler
//===============================================================================
void SIG_USART3_RECV(void) {
  u08 data;
  /* read the received data */
  data = UDR3;
  pUart[3]->rxFIFO.add(&data, 1);
}
#endif
