/****************************************************************************************/
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/wdt.h>
/****************************************************************************************/
extern "C" {
#include "types.h"
#include "delay.h"
#include "serial.h"
#include "socket.h"
#include "dhcp.h"
}
/****************************************************************************************/
#include "main.h"
#include "crc.h"
extern Ctag tags[MAX_NUM_TAGS];
/****************************************************************************************/
extern u_char SRC_MAC_ADDR[6];
extern u08* __bss_end;
/****************************************************************************************/
const u16 shortDispTime = 200;
const u16 medDispTime = (shortDispTime * 2);
const u16 longDispTime = (shortDispTime * 3);
u08 emu = 1;
/****************************************************************************************/
int main(void) {
  init();
  scheduler.attach(&rtcSignal);
  sei();
  pscreen = &welcome;
  //welcome.build();
  //welcome.display(1000);
  scheduler.attach(&screen.signal);
  scheduler.start();
//  configureNetwork();
//  configureReader();

  cc1101.enable();
  //wdt_enable(WDTO_2S);
  while (1) {
//    wdt_reset();
//    checkAutoconnect();
    checkTags();
//    checkDate();
//    pscreen->service();
  }
}
/****************************************************************************************/
void checkDate() {
  if (rtcSignal.isSet()) {
    rtc.getDate(&date);
  }
}
/****************************************************************************************/
void checkTags() {
  sRadioPacket pkt;
  tagTable.service();
  // If a new packet came in add it to the tag list.
  if (!cc1101.rxFifo.empty()) {
    cc1101.rxFifo.remove(&pkt, 1);
    network.tx(1, 0x11, (u08*) &pkt, sizeof(pkt));
//    outUart.send((c08*)&pkt,sizeof(pkt));
    //tagTable.add(&pkt);
    tagscreen.build(&pkt, tags[0].rssiOut, tags[0].rssiThreshold);
    tagscreen.display();
  }
  // Post new events to server
//  if (!tagTable.events.empty()) {
//    tagTable.events.remove(&event, 1);
//    server.setEvent(event);
//    tagscreen.build();
//    tagscreen.display();
//  }
}
///****************************************************************************************/
//void configureReader() {
//  u32 readerID;
//  screen.display("CONFIG READER ID", "START", shortDispTime);
//  if (!store.eeprom.readerConfigured) {
//    if (server.getReaderID(&readerID)) {
//      store.setReaderID(readerID);
//    }
//  }
//  screen.display("CONFIG READER ID", "DONE");
//}
///****************************************************************************************/
//void findServer() {
//  bool serverOK = false;
//  u08 str[screenMaxCol];
//  u08 ipNum = 0;
//  sServerConfig serverConfig;
//  sIP tryIP = store.eeprom.activeConfig.serverConfig.ip;
//  ipSocket.setDst(tryIP);
//  while (!serverOK && ipNum < numConfigs) {
//    if (server.getServerConfig(&serverConfig)) {
//      screen.ip2str(str, &tryIP);
//      screen.display("SERVER FOUND @", (const char*) str, 1000);
//      store.setServerConfig(serverConfig);
//      serverOK = true;
//    } else {
//      screen.ip2str(str, &tryIP);
//      screen.display("NO SERVER @", (const char*) str, 1000);
//      tryIP = defServerIP[ipNum];
//      ipSocket.setDst(tryIP);
//      ipNum++;
//    }
//  }
//  if (!serverOK) {
//    wdt_enable(WDTO_2S);
//    while (1) {
//    };
//  }
//}
///****************************************************************************************/
//void configureNetwork() {
//  sMacADR macAdr;
//  sNetConfig netConfig;
//  sServerConfig serverConfig;
//  u08 str[screenMaxCol];
//  screen.display("CONFIG NETWORK", "START", shortDispTime);
//  netInit();
//  bool res = true;
//
//  findServer();
//  if (!store.eeprom.networkConfigured) {
//    if (server.getServerConfig(&serverConfig)) {
//      store.setServerConfig(serverConfig);
//    } else {
//      res = false;
//    }
//    if (server.getMacADR(&macAdr)) {
//      store.setMacAdr(macAdr);
//    } else {
//      res = false;
//    }
//    if (server.getNetConfig(&netConfig)) {
//      store.setNetConfig(netConfig);
//    } else {
//      res = false;
//    }
//    if (res) {
//      store.setNetConf();
//      // Display reboot message
//      screen.display("POWER DOWN", "SWAP NETWORK");
//    } else {
//      // Display reboot message
//      screen.ip2str(str, &store.eeprom.activeConfig.serverConfig.ip);
//      screen.display("NO SERVER @", (const char*) str, 1000);
//    }
//    while (1) {
//    };
//  }
//  screen.display("CONFIG NETWORK", "DONE");
//}
///****************************************************************************************/
//bool getConfig() {
//  sConfig newConfig;
//// Get the most recent config from the server
//  if (!server.getConfig(&newConfig)) {
//    return false;
//  } else {
//    store.setConfig(newConfig);
//    return true;
//  }
//}
///****************************************************************************************/
//void checkSw() {
//  sSwBlock block;
//  c08 str[screenMaxCol];
//  u16 blockNum;
//  bool error = false;
//  u16 numBlocks = store.eeprom.activeConfig.sw.numBlocks;
//// Check if new software version is available
//  if (store.eeprom.activeConfig.sw.revision != store.flash.storedSw.revision) {
//    store.eraseSw();
//    block.meta.blockRevision = store.eeprom.activeConfig.sw.revision;
//    //cli();
//    for (blockNum = 0; blockNum < numBlocks; blockNum++) {
//      // Download new sw version
//      block.meta.blockRevision = store.eeprom.activeConfig.sw.revision;
//      block.meta.blockNumber = blockNum;
//      if (blockNum % 10 == 0) {
//        sprintf(str, "REV %i BLK %i", block.meta.blockRevision, (numBlocks - blockNum));
//        screen.display("SW DOWNLOADING", str);
//      }
//      if (server.getSwBlock(&block)) {
//        //screen.display("BLOCK SVR ", "READ");
//        store.setSwBlock(&block);
//        //screen.display("BLOCK FLSH ", "STORE");
//      } else {
//        screen.display("SW DOWNLOADING", "FAIL BLOCK", 2000);
//        error = true;
//        break;
//      }
//    }
//    if (!error) {
//      // Check the CRC
//      screen.display("SW DOWNLOADING", "CHECK CRC", 2000);
//      u16 calcCRC = 0;
//      store.calcSwCRC(&calcCRC, store.eeprom.activeConfig.sw);
//      if (calcCRC == store.eeprom.activeConfig.sw.crc) {
//        // Software CRC is fine.
//        store.setStoredSwMeta(store.eeprom.activeConfig.sw);
//        // reset
//        screen.display("SW DOWNLOADING", "DONE");
//        wdt_enable(WDTO_2S);
//        while (1) {
//        };
//      }
//      screen.display("SW DOWNLOADING", "FAIL CRC", 2000);
//    }
//    screen.display("SW DOWNLOADING", "FAIL DOWNLOAD", 2000);
//  }
//}
///****************************************************************************************/
//void setStatus() {
//  sStatus status;
//  status.autoConnectTime = store.eeprom.activeConfig.autoConnectTime;
//  status.readerID = store.eeprom.activeConfig.readerID;
//  status.state = mcu.errorState;
//  status.sw = store.flash.currentSw;
//  server.setStatus(status);
//}
//
///****************************************************************************************/
//void autoConnect() {
//  sDate date;
//  screen.display("AUTO CONNECT", "GET DATE", shortDispTime);
//  wdt_reset();
//  if (server.getDate(&date)) {
//    screen.display("AUTO CONNECT", "GET DATE DONE", shortDispTime);
//    rtc.setDate(&date);
//  } else {
//    screen.display("AUTO CONNECT", "GET DATE FAIL", shortDispTime);
//  }
//  wdt_reset();
//  screen.display("AUTO CONNECT", "GET CONFIG", shortDispTime);
//  if (getConfig()) {
//    screen.display("AUTO CONNECT", "GET CONFIG DONE", shortDispTime);
//  } else {
//    screen.display("AUTO CONNECT", "GET CONFIG FAIL", shortDispTime);
//  }
//  wdt_reset();
//  screen.display("AUTO CONNECT", "GET SW", shortDispTime);
//  checkSw();
//  wdt_reset();
////screen.display("AUTO CONNECT", "SET STATUS", shortDispTime);
////setStatus();
//  screen.display("AUTO CONNECT", "DONE", shortDispTime);
//}
///****************************************************************************************/
//void checkAutoconnect() {
//  static enum {
//    AFTER_BOOT = 0, DURING = 1, BEFORE = 2
//  } state = AFTER_BOOT;
//  u08 connectHour;
//  connectHour = store.eeprom.activeConfig.autoConnectTime.hour;
//  ipSocket.service();
//  switch (state) {
//    case AFTER_BOOT:
//      autoConnect();
//      wdt_reset();
//      //welcome.build();
//      //welcome.display();
//      ipSocket.enableAutoclose();
//      cc1101.enable();
//      state = BEFORE;
//      break;
//    case BEFORE:
//      if (date.hour == connectHour) {
//        wdt_reset();
//        autoConnect();
//        state = DURING;
//      }
//      break;
//    case DURING:
//      wdt_reset();
//      if (date.hour == connectHour + 1) {
//        state = BEFORE;
//      }
//      break;
//    default:
//      wdt_reset();
//      state = BEFORE;
//      break;
//  }
//}
/****************************************************************************************/
void init(void) {
  mcu_init();
  //uart_init(1, 2);
  for (u08 c = 0; c < 3; c++) {
    lcd.init();
  }
  rtc.start();
  rtc.getDate(&date);
  rtcSignal.setPeriod(2 / TICK_BASE);
//init_timer();
}
/****************************************************************************************/
void debug(void) {
  u08 *ptr;
  sRadioPacket packet;
  cc1101.enable();
  while (1) {
    if (cc1101.tagreceived) {
      cc1101.tagreceived = false;
      cc1101.rxFifo.remove(&packet, 1);
      ptr = (u08*) &packet;
      printf("\n\rTAG: ");
      for (int i = 0; i < packet.size; i++) {
        printf("%02X ", *(ptr + i));
      }
      printf(" RSSI=%d LQI=%d CNT=%d", packet.rssi, packet.lqi, (u08) packet.tag.count);
    }
  }
}
/****************************************************************************************/
bool tryDHCP() {
  u08 mac[6];
  mac[0] = store.eeprom.activeConfig.macAdr.mac5;
  mac[1] = store.eeprom.activeConfig.macAdr.mac4;
  mac[2] = store.eeprom.activeConfig.macAdr.mac3;
  mac[3] = store.eeprom.activeConfig.macAdr.mac2;
  mac[4] = store.eeprom.activeConfig.macAdr.mac1;
  mac[5] = store.eeprom.activeConfig.macAdr.mac0;
  screen.display("AUTO CONNECT", "DHCP TRY", shortDispTime);
  init_dhcp_client(mac, 0, 0, 0);
  //screen.display("AUTO CONNECT", "INIT DONE", shortDispTime);
  if (getIP_DHCPS(mac)) {
    getSHAR((u08*) &store.eeprom.activeConfig.macAdr);
    getGAR((u08*) &store.dhcpConfig.gateway);
    getSUBR((u08*) &store.dhcpConfig.netMask);
    getSIPR((u08*) &store.dhcpConfig.src);
    store.dhcpConfig.useDHCP = B_TRUE;
    screen.display("AUTO CONNECT", "DHCP PASS", shortDispTime);
    return true;
  }
  screen.display("AUTO CONNECT", "DHCP FAIL", shortDispTime);
  return false;
}
/****************************************************************************************/
void assignStatic() {
  u08 mem_conf[8] = { 8, 8, 8, 8, 8, 8, 8, 8 };
  iinchip_init(); //W5300 Chip Init
  _delay_ms(20);
  setSHAR((u08*) &store.eeprom.activeConfig.macAdr);
  setGAR((u08*) &store.eeprom.activeConfig.netConfig.gateway); //Set Gateway
  setSUBR((u08*) &store.eeprom.activeConfig.netConfig.netMask); //Set Subnet Mask
  setSIPR((u08*) &store.eeprom.activeConfig.netConfig.src); //Set My IP
  sysinit(mem_conf, mem_conf);
}
/****************************************************************************************/
void netInit(void) {
  if (store.eeprom.activeConfig.netConfig.useDHCP) {
    if (tryDHCP()) {
      return;
    }
  }
  screen.display("IP STATIC", "TRY", shortDispTime);
  assignStatic();
  screen.display("IP STATIC", "DONE", shortDispTime);
}
/****************************************************************************************/
void mcu_init(void) {
  cli();
  /*
   #ifndef __DEF_IINCHIP_INT__
   EICRA = 0x00;
   EICRB = 0x00;
   EIMSK = 0x00;
   EIFR = 0x00;
   #else
   EICRA = 0x00; // External Interrupt Control Register A clear
   EICRB = 0x02;// External Interrupt Control Register B clear // edge
   EIMSK = (1 << INT4);// External Interrupt Mask Register : 0x10
   EIFR = 0xFF;// External Interrupt Flag Register all clear
   DDRE &= ~(1 << INT4);// Set PE Direction
   PORTE |= (1 << INT4);// Set PE Default value
   #endif
   */
#if (ATMEGA128_NUM_WAIT == ATMEGA128_0WAIT)
  MCUCR = 0x80;
  XMCRA=0x40;
#elif (ATMEGA128_NUM_WAIT == ATMEGA128_1WAIT)
  MCUCR = 0xc0; // Enable external ram
  XMCRA = 0x40; // External Memory Control Register A :
// Low sector   : 0x1100 ~ 0x7FFF
// Upper sector : 0x8000 ~ 0xFFFF
#elif (ATMEGA128_NUM_WAIT == ATMEGA128_2WAIT )
      MCUCR = 0x80;
      XMCRA=0x42;
#elif ((ATMEGA128_NUM_WAIT == ATMEGA128_3WAIT)
      MCUCR = 0xc0;
      XMCRA=0x42;
#else
#error "unknown atmega128 number wait type"
#endif
  sei();

// enable interrupts
}
/****************************************************************************************/
extern "C" void __cxa_pure_virtual(void) {
  while (1) {
  };
}
/****************************************************************************************/
