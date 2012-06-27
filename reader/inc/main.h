#ifndef MAIN_H_
#define MAIN_H_

#include <avr/wdt.h>
#include "network.h"
#include "pin.h"
#include "Csocket.h"
//#include "Cserver.h"
#include "tagtable.h"
#include "CI2C.h"
#include "MCP7940x.h"
#include "CC1101.h"
#include "storage.h"
#include "mcu.h"
#include "at45db041.h"
#include "scheduler.h"
#include "Cscreen.h"
#include "fir.h"
#include "CUART.h"
#include "network.h"

#define IICHIP_RESET_INIT()         (DDRE |= 0x40)
#define IICHIP_RESET()              (PORTE |= 0x40)

#define MY_NET_MEMALLOC     0x55          // MY iinchip memory allocation
#define MY_LISTEN_PORT      5000          // MY Listen Port  : 5000
#define SOCK_TCPC           0
#define SOCK_TCPS           1
#define DEFAULT_HTTP_PORT   80
/****************************************************************************************/
using namespace C1101;
using namespace CNETWORK;
/****************************************************************************************/
extern u08 debugSend;
/****************************************************************************************/
void mainService(void);
void DisplayConfig(void);
void netInit(void);
void mcu_init(void);
void autoConnect(void);
void init(void);
void debug(void);
void checkSw();
void checkAutoconnect();
void checkDate();
void checkTags();
void configureReader();
void configureNetwork();
/****************************************************************************************/
static sRtcTime date;
sEvent02 event;
/****************************************************************************************/
Cmcu mcu;
//CUART outUart(1, 2400, 64);
Csignal rtcSignal;
CI2C i2c;
Crtc rtc(&i2c, 0x6F);
Cat45db041 flash;
Cstorage store(&flash);
CC1101 cc1101(store.eeprom.activeConfig.rf.adr, store.eeprom.activeConfig.rf.channel);
Clcd lcd;
CDisplay display(&lcd);
Cscreen* pscreen;
Cscreen screen(&display, &store, &date);
Cwelcomescreen welcome(&display, &store, &date);
Ctagscreen tagscreen(&display, &store, &date);
Csocket ipSocket(1, store.eeprom.activeConfig.serverConfig.port,
                 store.eeprom.activeConfig.serverConfig.ip, bufSize);
//CNetwork ipNetwork(&ipSocket, bufSize);
//Cserver server(&ipNetwork, &store);
Ctagtable tagTable(&rtc, store.eeprom.activeConfig.readerID, &cc1101, &store);
//CNetwork network(&outUart, 32, 0x1);
Cpin pinSendSMS(ePORT_ATMEGA_128_B, 7, ePinOut, true);
/****************************************************************************************/
#endif /* MAIN_H_ */
