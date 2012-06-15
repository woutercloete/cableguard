/****************************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
/****************************************************************************************/
#include "types.h"
#include "avrlibdefs.h"
#include "uart.h"
#include "iopins.h"
#include "common.h"
/****************************************************************************************/
Cuart* pUart[NUM_UARTS];
/****************************************************************************************/
Cuart::Cuart(u08 uartNr, u16 _rxBufSize = 0, _txBufSize = 0, Cpin* _pinRTS = 0,
             Cpin* _pinCTS = 0) {
  if (uartNr < NUM_UARTS) {
    this->baseAdr = 0xC0 + (8 * uartNr);
    if (uartNr == 3)
      this->baseAdr = 0x130;
    this->pinRTS = pinRTS;
    this->pinCTS = pinCTS;
    rxFIFO.setBufSize(_rxBufSize);
    txFIFO.setBufSize(_txBFufSize);
    // enable RxD/TxD and interrupts
    (*(volatile u08*) (UCSRXB_ADR)) = BV(RXCIE0) | BV(TXCIE0) | BV(RXEN0) | BV(TXEN0);
    pUart[uartNr] = this;
    rxOverflowCnt = 0;
    if (pinRTS)
      pinRTS->disable();
    txBusy = false;
  }
}
/****************************************************************************************/
bool Cuart::start(void) {
  // enable RxD/TxD and interrupts
  if (healthy) {
    (*(volatile u08*) (UCSRXB_ADR)) = BV(RXCIE0) | BV(TXCIE0) | BV(RXEN0) | BV(TXEN0);
    return true;
  }
  return false;
}
/****************************************************************************************/
void Cuart::setPins(Cpin* pinRTS, Cpin* pinCTS) {
  this->pinRTS = pinRTS;
  this->pinCTS = pinCTS;
}
/****************************************************************************************/
void Cuart::setBufSize(u16 bufSize) {
  healthy = (rxFIFO.setBufSize(bufSize) && txFIFO.setBufSize(bufSize));
}
/****************************************************************************************/
void Cuart::clearRx(void) {
  rxFIFO.clear();
}
/****************************************************************************************/
u16 Cuart::send(c08* buffer, u16 nBytes) {
  u16 res;
  if (!nBytes || !buffer) {
    return 0;
  }
  res = txFIFO.add((u08 *) buffer, nBytes);
  (*(volatile u08*) (UCSRXB_ADR)) |= BV(UDRIE0);
  //return number of bytes written
  return res;
}
/****************************************************************************************/
u16 Cuart::receive(u08* buffer, u16 nBytes) {
  return rxFIFO.remove(buffer, nBytes);
}
/****************************************************************************************/
u16 Cuart::peek(c08* buffer) {
  return rxFIFO.peek(buffer);
}
/****************************************************************************************/
u16 Cuart::space(void) {
  return txFIFO.space();
}
/****************************************************************************************/
u16 Cuart::rxnum(void) {
  return rxFIFO.received();
}
/****************************************************************************************/
void Cuart::setBaudRate(u32 baudRate) {
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
  UBRRXH = U16_HI(u16UBRR);
  UBRRXL = U16_LO(u16UBRR);
}
/****************************************************************************************/
void Cuart::setAsync(void) {
  cbi(UCSRXC, UMSEL00);
  cbi(UCSRXC, UMSEL01);
}
/****************************************************************************************/
void Cuart::setSync(void) {
  sbi(UCSRXC, UMSEL00);
  cbi(UCSRXC, UMSEL01);
}
/****************************************************************************************/
void Cuart::setParityOff(void) {
  cbi(UCSRXC, UPM00);
  cbi(UCSRXC, UPM01);
}
/****************************************************************************************/
void Cuart::setParityOdd(void) {
  sbi(UCSRXC, UPM00);
  sbi(UCSRXC, UPM01);
}
/****************************************************************************************/
void Cuart::setParityEven(void) {
  cbi(UCSRXC, UPM00);
  sbi(UCSRXC, UPM01);
}
/****************************************************************************************/
void Cuart::setStop1(void) {
  cbi(UCSRXC, USBS0);
}
/****************************************************************************************/
void Cuart::setStop2(void) {
  sbi(UCSRXC, USBS0);
}
/****************************************************************************************/
void Cuart::setCharSize(eCharSize size) {
  UCSRXB &= (~(size & 0x4) << UCSZ02);
  UCSRXB |= ((size & 0x4) << UCSZ02);
  UCSRXC &= (~(size & 0x3) << UCSZ00);
  UCSRXC |= ((size & 0x3) << UCSZ00);
}
/****************************************************************************************/
void Cuart::loadUDR(void) {
  u08 dat;
  u08 cnt;
  if (pinRTS != 0) {
    pinRTS->enable();
  }
  if (this->baseAdr == 0x130) {
    cnt = 0;
  }
  if (pinCTS == 0 || (pinCTS != 0 && pinCTS->enabled())) {
    cnt = txFIFO.remove(&dat, 1);
    if (cnt) {
      UDRX = dat;
    } else {
      cbi(UCSRXB, UDRIE0); /* disable UDRE interrupt */
    }
  }
}
/****************************************************************************************/
void Cuart::endTx(void) {
  if (pinRTS != 0) {
    pinRTS->disable();
  }
}
/****************************************************************************************/
void Cuart::endRx(void) {
  u08 data;
  /* read the received data */
  data = UDRX;
  rxFIFO.add(&data, 1);
}
/****************************************************************************************/
void Cuart::sendStr_P(const prog_char str[]) {
  register char c;
  if (!str)
    return;
  // print the string until the null-terminator
  while ((c = pgm_read_byte(str++)))
    send(&c, 1);
}
/****************************************************************************************/
void Cuart::sendStr(c08* str) {
  send(str, strlen(str));
}
/****************************************************************************************/
u16 Cuart::send_P(const prog_char buf[], u16 nBytes) {
  u16 res;
  u08 dat;
  if (!nBytes || !buf) {
    return 0;
  }
  while (nBytes) {
    dat = pgm_read_byte(buf++);
    res = txFIFO.add(&dat, 1);
    nBytes--;
  }
  return res;
}
/****************************************************************************************/
void Cuart::uprintf(const char *__fmt, ...) {
  c08 str[255];
  va_list arg;
  va_start(arg, __fmt);
  vsprintf(str, __fmt, arg);
  va_end(arg);
  send(str, strlen(str));
}
/****************************************************************************************/
// ISRs
/****************************************************************************************/
#ifdef UDR0
ISRS(0)
#endif
#ifdef UDR1
ISRS(1)
#endif
#ifdef UDR2
ISRS(2)
#endif
#ifdef UDR3
ISRS(3)
#endif
