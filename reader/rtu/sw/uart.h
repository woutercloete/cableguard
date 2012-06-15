#ifndef CUART_H
#define CUART_H

#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "fifo.h"
#include "pin.h"

typedef enum {
  eSize5 = 0, eSize6 = 1, eSize7 = 2, eSize8 = 3, eSize9 = 7,
} eCharSize;

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
#ifdef UCSR3B
#define NUM_UARTS 4
#else
#ifdef UCSR2B
#define NUM_UARTS 3
#else
#ifdef UCSR1B
#define NUM_UARTS 2
#else
#ifdef UCSR0B
#define NUM_UARTS 1
#endif
#endif
#endif
#endif
/****************************************************************************************/
#define UCSRXA_ADR	(this->baseAdr)
#define UCSRXB_ADR  (this->baseAdr+1)
#define UCSRXC_ADR  (this->baseAdr+2)
#define UBRRXL_ADR  (this->baseAdr+4)
#define UBRRXH_ADR  (this->baseAdr+5)
#define UDRX_ADR   	(this->baseAdr+6)
/****************************************************************************************/
#define UCSRXA		_MMIO_BYTE(UCSRXA_ADR)
#define UCSRXB		_MMIO_BYTE(UCSRXB_ADR)
#define UCSRXC		_MMIO_BYTE(UCSRXC_ADR)
#define UBRRXL		_MMIO_BYTE(UBRRXL_ADR)
#define UBRRXH		_MMIO_BYTE(UBRRXH_ADR)
#define UDRX		_MMIO_BYTE(UDRX_ADR)
/****************************************************************************************/
// ISRs
/****************************************************************************************/
#define ISRS(X) \
void USART##X##_UDRE_vect(void) { \
	pUart[X]->loadUDR(); \
} \
void USART##X##_TX_vect(void) { \
	pUart[X]->endTx(); \
} \
void USART##X##_RX_vect(void) { \
	pUart[X]->endRx(); \
} \
/****************************************************************************************/
#ifdef SIG_UART_DATA
#warning UART REGISTERS FOR MEGA 8
extern "C" void SIG_UART_DATA(void) __attribute__ ((signal));
extern "C" void SIG_UART_TRANS(void) __attribute__ ((signal));
extern "C" void SIG_UART_RECV(void) __attribute__ ((signal));
#endif

#ifdef SIG_USART_DATA
#warning UART REGISTERS FOR MEGA 88
extern "C" void SIG_USART_DATA(void) __attribute__ ((signal));
extern "C" void SIG_USART_TRANS(void) __attribute__ ((signal));
extern "C" void SIG_USART_RECV(void) __attribute__ ((signal));
#endif

#ifdef SIG_USART0_DATA
extern "C" void SIG_USART0_DATA(void) __attribute__ ((signal));
extern "C" void SIG_USART0_TRANS(void) __attribute__ ((signal));
extern "C" void SIG_USART0_RECV(void) __attribute__ ((signal));
#endif

#ifdef UDR1
extern "C" void SIG_USART1_DATA(void) __attribute__ ((signal));
extern "C" void SIG_USART1_TRANS(void) __attribute__ ((signal));
extern "C" void SIG_USART1_RECV(void) __attribute__ ((signal));
#endif

#ifdef UDR2
extern "C" void SIG_USART2_DATA(void) __attribute__ ((signal));
extern "C" void SIG_USART2_TRANS(void) __attribute__ ((signal));
extern "C" void SIG_USART2_RECV(void) __attribute__ ((signal));
#endif

#ifdef UDR3
extern "C" void SIG_USART3_DATA(void) __attribute__ ((signal));
extern "C" void SIG_USART3_TRANS(void) __attribute__ ((signal));
extern "C" void SIG_USART3_RECV(void) __attribute__ ((signal));
#endif

class Cuart {
  private:
    u16 rxOverflowCnt;
    u08 txCnt;
    u16 lastTxTime;
    u16 baseAdr;
  public:
    Cpin* pinRTS;
    Cpin* pinCTS;
    Tfifo<u08> rxFIFO;
    Tfifo<u08> txFIFO;
    u08 txBusy;
    u32 baudRate;
    Cuart(u08 uartNr);
    Cuart(u08 uartNr, u16 bufSize, Cpin* pinRTS, Cpin* pinCTS);
    u16 send(c08* buffer, u16 nBytes);
    u16 receive(u08* buffer, u16 nBytes);
    u16 peek(c08* buffer);
    u16 space(void);
    u16 rxnum(void);
    bool start(void);
    void clearRx(void);
    void setAsync(void);
    void setBaudRate(u32 baudrate);
    void setBufSize(u16 bufsize);
    void setCharSize(eCharSize size);
    void setParityOff(void);
    void setParityOdd(void);
    void setParityEven(void);
    void setPins(Cpin* pinRTS, Cpin* pinCTS);
    void setStop1(void);
    void setStop2(void);
    void setSync(void);
    void loadUDR(void);
    void endTx(void);
    void endRx(void);
    u16 send_P(const prog_char buf[], u16 nBytes);
    void sendStr_P(const prog_char str[]);
    void sendStr(c08* str);
    void uprintf(const char *__fmt, ...);
};

#endif

