/****************************************************************************************/
// This class contains all the visible tags and an event table.
//
//   Created : 15 March 2012
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
/****************************************************************************************/
#ifndef TAGTABLE_H
#define TAGTABLE_H
/****************************************************************************************/
#include "tag.h"
#include "fifo.h"
#include "payload.h"
#include "ds3231.h"
#include "scheduler.h"
#include "types.h"
#include "uart.h"
/****************************************************************************************/
extern Cuart DbgUart;
/****************************************************************************************/
namespace TAG_TABLE {
  const u08 MAX_NUM_TAGS = 8;
  const u08 MAX_NUM_EVENTS = 8;
  const u16 SERVICE_PERIOD = 1; // Service period in seconds
  //const u16 SERVICE_TICKS = SERVICE_PERIOD / SCHEDULER::TICK_BASE;
  const u16 OUT_RANGE_TIMEOUT = 10; // Seconds before tag is considered out of rssiThreshold
  const u16 OUT_RANGE_TICKS = OUT_RANGE_TIMEOUT / SERVICE_PERIOD;
}
/****************************************************************************************/
using namespace TAG;
using namespace TAG_TABLE;
/****************************************************************************************/
typedef struct _sRadioPacket {
    u08 size;
    sRfTag tag;
    u08 rssi;
    u08 lqi;
    bool crc_ok;
} sRadioPacket;
/****************************************************************************************/
typedef sTimeDate sRtcTime;
typedef sRtcTime sDate;
/****************************************************************************************/
typedef struct {
    sTag tag;
    u32 readerID;
    TAG::eEvent eventType;
    sDate date;
} sEvent;
/****************************************************************************************/
extern Ctag tagList[MAX_NUM_TAGS];
/****************************************************************************************/
class Ctagtable {
  private:
    Crtc* rtc;
    u32 readerID;
    u32 lostCnt;
    Csignal signal;
    u08 cntFilter;
  public:
    volatile u32 timer;
    Tfifo<sEvent> events;
    /****************************************************************************************/
    Ctagtable(Crtc* _rtc, u32 _readerID) {
      events.setBufSize(MAX_NUM_EVENTS);
      readerID = _readerID;
      lostCnt = 0;
      rtc = _rtc;
      //signal.setPeriod(TAG_TABLE::SERVICE_TICKS);
      //scheduler.attach(&signal);
    }
    /****************************************************************************************/
    bool add(sRadioPacket* radioPacket) {
      sTag tag;
      u08 cnt;
      // Check if tag is not already in tags
      for (cnt = 0; cnt < MAX_NUM_TAGS; cnt++) {
        if (tagList[cnt].isUsed()) {
          if (radioPacket->tag.tagID == tagList[cnt].getTagID()) {
            // Update tag
            tag.rssi = radioPacket->rssi;
            tag.lqi = radioPacket->lqi;
            tag.rfTag = radioPacket->tag;
            tagList[cnt].update(&tag);
            return false;
          }
        }
      }
      // Tag is not in tags yet add it.
      for (cnt = 0; cnt < MAX_NUM_TAGS; cnt++) {
        if (!tagList[cnt].isUsed()) {
          tag.rssi = radioPacket->rssi;
          tag.lqi = radioPacket->lqi;
          tag.rfTag = radioPacket->tag;
          tagList[cnt].setNew(&tag, 150);
          return true;
        }
      }
      return true;
    }
    /****************************************************************************************/
    void service(void) {
      c08 str[64];
      bool signal = false;
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (timer > 4000000) {
          signal = true;
          timer = 0;
        }
      }
      if (signal) {
        if (cntFilter == 3) {
          for (u08 cnt = 0; cnt < MAX_NUM_TAGS; cnt++) {
            if (tagList[cnt].isUsed()) {
              tagList[cnt].clock();
            }
          }
          cntFilter = 0;
        } else {
          cntFilter++;
        }
        for (u08 cnt = 0; cnt < MAX_NUM_TAGS; cnt++) {
          if (tagList[cnt].isUsed()) {
              snprintf(str, 64, "\r\nTAG ID : %d RSSI %d %d MOV: %d", (u08)tagList[cnt].serverTag.rfTag.tagID,
                       (u08)tagList[cnt].serverTag.rssi, (u08)tagList[cnt].rssiOut, tagList[cnt].movementNow);
              DbgUart.send(str, strlen(str));
            if (tagList[cnt].state == TAG::VISIBLE) {
              if (tagList[cnt].isOutRange()) {
                sEvent event;
                sRtcTime date;
                //rtc->getDate(&date);
                event.tag = tagList[cnt].serverTag;
                event.readerID = readerID;
                event.eventType = TAG::OUT_RANGE;
                event.date = date;
                events.add(&event, 1);
                tagList[cnt].setUnused();

              }
              if (tagList[cnt].startedMoving) {
                sEvent event;
                sRtcTime date;
                //rtc->getDate(&date);
                event.tag = tagList[cnt].serverTag;
                event.readerID = readerID;
                event.eventType = TAG::MOVEMENT_CHANGED;
                event.date = date;
                events.add(&event, 1);
              }
            } else if (tagList[cnt].state == TAG::NEW) {
              if (tagList[cnt].isInRange()) {
                sRtcTime date;
                sEvent event;
                //rtc->getDate(&date);
                event.tag = tagList[cnt].serverTag;
                event.readerID = readerID;
                event.eventType = TAG::IN_RANGE;
                event.date = date;
                events.add(&event, 1);
                tagList[cnt].state = TAG::VISIBLE;
              }
            }
          }
        }
      }
    }
};
/****************************************************************************************/
#endif

