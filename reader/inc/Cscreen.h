/*
 * Cscreen.h
 *
 *  Created on: 06 Mar 2012
 *      Author: wouter
 */
/****************************************************************************************/
#ifndef CSCREEN_H_
#define CSCREEN_H_
/****************************************************************************************/
#include <util/delay.h>
#include "avrlibtypes.h"
#include "CDisplay.h"
#include "storage.h"
#include "scheduler.h"
#include "cc1101.h"
/****************************************************************************************/
#define DISPLAY_TIMEOUT 5
/****************************************************************************************/
extern sEvent02 event;
/****************************************************************************************/
const u08 screenMaxCol = (LCD_MAX_COL);
/****************************************************************************************/
typedef struct {
    union {
        c08 lines[LCD_MAX_ROW][screenMaxCol];
        c08 line[LCD_MAX_ROW * screenMaxCol];
    };
} sCurrentScreen;
/****************************************************************************************/
class Cscreen {
  protected:
    static CDisplay* displayInterface;
    static u08 displayTimer;
    static sCurrentScreen screen;
    static sRtcTime* date;
    static Cstorage* store;
    static u08 idx[LCD_MAX_ROW];
    static u08 size[LCD_MAX_ROW];
    void mac2str(u08* str, sMacADR* mac);
  public:
    static Csignal signal;
    Cscreen() {
    }
    Cscreen(CDisplay* display, Cstorage* store, sRtcTime* date) {
      displayInterface = display;
      displayTimer = 0;
      this->store = store;
      this->date = date;
      idx[0] = 0;
      idx[1] = 0;
      signal.setPeriod(1 / TICK_BASE); // 3907 * 256us = 1.000192s);
    }
    void ip2str(u08* str, sIP* ip);
    void service() {
      if (signal.isSet()) {
        build();
        display();
      }
    }
    virtual void build(void);
    virtual void display(void);
    void display(u16 delay_ms) {
      display();
      _delay_ms(delay_ms);
    }
    void display(const c08* str1, u16 delay_ms = 0) {
      memset(&screen, 32, sizeof(screen));
      if (str1 != 0)
        snprintf((c08*) &screen.lines[0][0], screenMaxCol, "%s", str1);
      build();
      display();
      _delay_ms(delay_ms);
    }
    void display(const c08* str1, const c08* str2 = 0, u16 delay_ms = 0) {
      memset(&screen, 32, sizeof(screen));
      if (str1 != 0)
        snprintf((c08*) &screen.lines[0][0], screenMaxCol, "%s", str1);
      if (str2 != 0)
        snprintf((c08*) &screen.lines[1][0], screenMaxCol, "%s", str2);
      build();
      display();
      _delay_ms(delay_ms);
    }
};
/****************************************************************************************/
class Cwelcomescreen: public Cscreen {
  private:
    enum {
      DATE = 0, SERVER = 1, PORT = 2, IP = 3, MASK = 4, DHCP = 5, REVISION = 6
    } screenNum;
  public:
    Cwelcomescreen(CDisplay* display, Cstorage* store, sRtcTime* date) :
        Cscreen(display, store, date) {
      screenNum = DATE;
    }
    void build(void) {
      static u08 str[screenMaxCol];
      sNetConfig* conf;
      conf =
          (store->eeprom.activeConfig.netConfig.useDHCP) ? &store->dhcpConfig :
                                                           &store->eeprom.activeConfig.netConfig;
      memset(&screen, 32, sizeof(screen));
      snprintf((c08*) &screen.lines[0][0], screenMaxCol, "READER ID %d",
               (u16) store->eeprom.activeConfig.readerID);
      switch (screenNum) {
        case DATE:
          snprintf((c08*) &screen.lines[1][0], screenMaxCol, "%04d:%02d:%02d %02d:%02d",
                   date->year, date->mon, date->day, date->hour, date->min);
          screenNum = SERVER;
          break;
        case SERVER:
          ip2str(str, &store->eeprom.activeConfig.serverConfig.ip);
          snprintf((c08*) &screen.lines[1][0], screenMaxCol, "SERV:%s", str);
          screenNum = PORT;
          break;
        case PORT:
          snprintf((c08*) &screen.lines[1][0], screenMaxCol, "SERV PORT:%i",
                   store->eeprom.activeConfig.serverConfig.port);
          screenNum = IP;
          break;
        case IP:
          ip2str(str, &conf->src);
          snprintf((c08*) &screen.lines[1][0], screenMaxCol, "IP:%s", str);
          screenNum = MASK;
          break;
        case MASK:
          ip2str(str, &conf->netMask);
          snprintf((c08*) &screen.lines[1][0], screenMaxCol, "%s", str);
          screenNum = DHCP;
          break;
        case DHCP:
          if (conf->useDHCP)
            snprintf((c08*) &screen.lines[1][0], screenMaxCol, "DHCP: ON");
          else
            snprintf((c08*) &screen.lines[1][0], screenMaxCol, "DHCP: OFF");
          screenNum = REVISION;
          break;
        case REVISION:
          snprintf((c08*) &screen.lines[1][0], screenMaxCol, "SW REV:%i",
                   store->flash.currentSw.revision);
          screenNum = DATE;
          break;
      }
      Cscreen::build();
    }
};
#if 0
/****************************************************************************************/
class Cnetworkscreen: public Cscreen {
  private:
  public:
  Cnetworkscreen(CDisplay* display, Cstorage* store, sRtcTime* date) :
  Cscreen(display, store, date) {
  }
  void build(void) {
    u08 strIP[16];
    u08 strMask[16];
    u08 strGateway[16];
    u08 strDns[16];
    u08 strServerIP[16];

    memset(&screen, 32, sizeof(screen));
    ip2str(strIP, &store->eeprom.activeConfig.netConfig.src, 16);
    ip2str(strMask, &store->eeprom.activeConfig.netConfig.netMask, 16);
    ip2str(strGateway, &store->eeprom.activeConfig.netConfig.gateway, 16);
    ip2str(strDns, &store->eeprom.activeConfig.netConfig.dns, 16);
    ip2str(strServerIP, &store->eeprom.activeConfig.serverConfig.ip, 16);
    snprintf((c08*) &screen.lines[0][0], "DHCP:%d IP:%s Mask:%s Gateway:%s DNS:%s",
        store->eeprom.activeConfig.netConfig.useDHCP, strIP, strMask, strGateway, strDns);
    snprintf((c08*) &screen.lines[1][0], "SERVER IP:%s SERVER PORT:%4d", strServerIP,
        store->eeprom.activeConfig.serverConfig.port);
    Cscreen::build();
  }
};
#endif
/****************************************************************************************/
class Ctagscreen: public Cscreen {
  private:
  public:
    /****************************************************************************************/
    Ctagscreen(CDisplay* display, Cstorage* store, sRtcTime* date) :
        Cscreen(display, store, date) {
    }
    /****************************************************************************************/
    void build(C1101::sRadioPacket* pkt, u08 filRssi, u08 rssiReject) {
      u08 strEvent[16];
      if (event.eventType == TAG::IN_RANGE) {
        strcpy((c08*) strEvent, "IN RANGE");
      } else {
        strcpy((c08*) strEvent, "OUT RANGE");
      }
      memset(&screen, 32, sizeof(screen));
      snprintf((c08*) &screen.lines[0][0], screenMaxCol, "#%04X%04X",
               (u16) (pkt->tag.tagID >> 16), (u16) pkt->tag.tagID);
      //snprintf((c08*) &screen.lines[1][0], "RSSI: %d %d", pkt->rssi, pkt->tag.count);
      snprintf((c08*) &screen.lines[1][0], screenMaxCol, "RSSI: %d %d %d", pkt->rssi,
               filRssi, rssiReject);
      Cscreen::build();
    }
    /****************************************************************************************/
    void build(void) {
      u08 strEvent[16];
      switch (event.eventType) {
        case TAG::IN_RANGE:
          strcpy((c08*) strEvent, "IN RANGE");
          break;
        case TAG::OUT_RANGE:
          strcpy((c08*) strEvent, "OUT RANGE");
          break;
        case TAG::MOVEMENT_CHANGED:
          strcpy((c08*) strEvent, "MOVED");
          break;
      }
      memset(&screen, 32, sizeof(screen));
      snprintf((c08*) &screen.lines[0][0], screenMaxCol, "TAG %s", strEvent);
      snprintf((c08*) &screen.lines[1][0], screenMaxCol, "#%04X%04X",
               (u16) (event.tag.rfTag.tagID >> 16), (u16) event.tag.rfTag.tagID);
      Cscreen::build();
    }
};
/****************************************************************************************/
#endif /* CSCREEN_H_ */
