/****************************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <util/atomic.h>
#include <util/delay.h>
/****************************************************************************************/
#include "main.h"
#include "sim300.h"
#include "timer.h"
#include "uart.h"
#include "i2c.h"
#include "ds3231.h"
#include "controller.h"
#include "tagtable.h"
#include "network.h"
/****************************************************************************************/
static volatile u32 isr_time = 0;
static volatile u32 time = 0;
/****************************************************************************************/
#define TICKER_PERIOD  139;
using namespace CONTROLLER;
using namespace NETWORK;
/****************************************************************************************/
void TimeTicker(void);
/****************************************************************************************/
//CTimer Timer(0, TIMER_CLK_DIV8);
//Cuart DbgUart(0, 115200, 512);
//Cuart rfidUart(1, 2400, 64);
//Cuart ModemUart(2, 115200, 512);
//CNetwork network(&rfidUart, 32, 0x11);
//Csim300 Modem(&ModemUart);
//CI2C i2c;
//Crtc rtc(&i2c, 0xD0);
//CServer Server(&Modem);
//CRFID Rfid(&rfidUart, 16, &Server);
//Ctagtable tagTable(&rtc, 1);
//CController Controller(&Rfid, &Modem, &Server, &tagTable, &network);
//sTimeDate rtc_time;
/****************************************************************************************/
int main(void) {


//  InitIOPins();
//  WDTCSR = 0x00; // Disable Watchdog for now
//  Timer.attach(TimeTicker);
//  _delay_ms(500);
//  sei();
//
//  DbgUart.sendStr("\x1B[2J"); //Clear Screen
//  DbgUart.sendStr("\x1B[0;0H"); //Position Cursor
//  DbgUart.sendStr("\n\r  ===== RFID MANAGER CABLE DEMO ===== \n\r");
//  DbgUart.sendStr(rtc.getTimestamp());
//
//  Controller.Setup();
//
//  while (1) {
//    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
//      time = isr_time;
//    }
//    tagTable.service();
//    network.service();
//    Controller.Service();
//    if (time > 200000) {
//      time = 0;
//
//      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
//        isr_time = 0;
//      }
//      LED_3_TOGGLE();
//      if (Modem.simcard_ok) {
//        LED_0_TOGGLE();
//      } else {
//        LED_0_OFF();
//      }
//      if (Modem.signal_ok) {
//        LED_1_TOGGLE();
//      } else {
//        LED_1_OFF();
//      }
//      if (Modem.connect_ok) {
//        LED_2_TOGGLE();
//      } else {
//        LED_2_OFF();
//      }
//    }
//  }
  return 0;
}
/****************************************************************************************/
void TimeTicker(void) {
//  Rfid.timer += TICKER_PERIOD;
//  isr_time += TICKER_PERIOD;
//  Modem.isr_timer += TICKER_PERIOD;
//  Controller.timer += TICKER_PERIOD;
//  tagTable.timer += TICKER_PERIOD;
}
/****************************************************************************************
 C++ work around
 ****************************************************************************************/
extern "C" void __cxa_pure_virtual(void) {
  // call to a pure virtual function happened ... wow, should never happen ... stop
  while (1)
    ;
}
