/****************************************************************************************/
// The Cserial class is the interface definition for serial driver classes.
//
//   Created : 29 Feb 2012
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
/****************************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
/****************************************************************************************/
#include "serial.h"
/****************************************************************************************/
Cserial::Cserial(void) {

}
/****************************************************************************************/
void Cserial::service(void) {

}
/****************************************************************************************/
void Cserial::clearRx(void) {
  rxFIFO.clear();
}
/****************************************************************************************/
void Cserial::setBufSize(u16 bufSize) {
  rxFIFO.setBufSize(bufSize);
  txFIFO.setBufSize(bufSize);
}
/****************************************************************************************/
u16 Cserial::receive(u08* buffer, u16 nBytes) {
  return rxFIFO.remove(buffer, nBytes);
}
/****************************************************************************************/
u16 Cserial::receive(u08* buffer) {
  return rxFIFO.remove(buffer);
}
/****************************************************************************************/
u16 Cserial::space(void) {
  return txFIFO.space();
}
/****************************************************************************************/
u16 Cserial::rxnum(void) {
  return rxFIFO.used();
}
/****************************************************************************************/
void Cserial::uprintf(const char *__fmt, ...) {
  c08 str[255];
  va_list arg;
  va_start(arg, __fmt);
  vsprintf(str, __fmt, arg);
  va_end(arg);
  send((u08*) str, strlen(str));
}
/****************************************************************************************/
void Cserial::sendStr_P(const prog_char str[]) {
  register char c;
  if (!str)
    return;
  // print the string until the null-terminator
  while ((c = pgm_read_byte(str++)))
    send((u08*) &c, 1);
}
/****************************************************************************************/
void Cserial::sendStr(c08* str) {
  send((u08*) str, strlen(str));
}
/****************************************************************************************/
u16 Cserial::send_P(const prog_char buf[], u16 nBytes) {
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
void Cserial::setBaudRate(u32 baudRate) {
  this->baudRate = baudRate;
}
/****************************************************************************************/

