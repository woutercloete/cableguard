/****************************************************************************************/
// This class contains all the visible tags and an event tags.
//
//   Created : 15 March 2012
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
/****************************************************************************************/
#include <string.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include <util/delay.h>
/****************************************************************************************/
#include "tagtable.h"
#include "CUART.h"
/****************************************************************************************/
using namespace C1101;
extern CUART outUart;
/****************************************************************************************/
Ctag tags[MAX_NUM_TAGS];
extern CUART outUart;
const u08 DEBUG_UART_BUF_SIZE = 164;
/****************************************************************************************/
u32 convDB(u08 rssi);
/****************************************************************************************/
const u08 TAMPER_RESEND = 60;
/****************************************************************************************/
bool Ctagtable::add(C1101::sRadioPacket* radioPacket) {
  sServerTag02 serverTag;
  u08 cnt;
  // Check if tag is not already in tags
  for (cnt = 0; cnt < MAX_NUM_TAGS; cnt++) {
    if (tags[cnt].isUsed()) {
      if (radioPacket->tag.tagID == tags[cnt].getTagID()) {
        // Update tag
        tags[cnt].tamper = radioPacket->tag.tamper;
        serverTag.rssi = radioPacket->rssi;
        serverTag.lqi = radioPacket->lqi;
        serverTag.rfTag = radioPacket->tag;
        tags[cnt].update(&serverTag);
        return false;
      }
    }
  }
  // Tag is not in tags yet add it.
  for (cnt = 0; cnt < MAX_NUM_TAGS; cnt++) {
    if (!tags[cnt].isUsed()) {
      tags[cnt].tamper = radioPacket->tag.tamper;
      serverTag.rssi = radioPacket->rssi;
      serverTag.lqi = radioPacket->lqi;
      serverTag.rfTag = radioPacket->tag;
      tags[cnt].setNew(&serverTag, defRssiThreshold);
      return true;
    }
  }
  return true;
}
/****************************************************************************************/
void Ctagtable::service(void) {
  c08 str[DEBUG_UART_BUF_SIZE];
  c08 finalStr[DEBUG_UART_BUF_SIZE];
  if (signal.isSet()) {
    // Debug print
    //outUart.rxFIFO.clear();
    snprintf(finalStr, DEBUG_UART_BUF_SIZE, "\r\n");
    for (u08 cnt = 0; cnt < MAX_NUM_TAGS; cnt++) {
      if (tags[cnt].isUsed()) {
        snprintf(str, DEBUG_UART_BUF_SIZE, "#%04X %3d %3d %3d %3d ",
                 (u16) tags[cnt].serverTag.rfTag.tagID, tags[cnt].serverTag.rfTag.count,
                 tags[cnt].movementNow, tags[cnt].tamper, tags[cnt].rssiOut);
        strcat(finalStr, str);
      }
    }
    outUart.sendStr((c08*) finalStr);
    if (cntFilter == 3) {
      for (u08 cnt = 0; cnt < MAX_NUM_TAGS; cnt++) {
        if (tags[cnt].isUsed()) {
          tags[cnt].clock();
        }
      }
      cntFilter = 0;
    } else {
      cntFilter++;
    }
    for (u08 cnt = 0; cnt < MAX_NUM_TAGS; cnt++) {
      if (tags[cnt].isUsed()) {
        if (tags[cnt].state == TAG::VISIBLE) {
          if (tags[cnt].tamper) {
            if (tags[cnt].tamperCnt == 0) {
              XREADER::sEvent02 event;
              sRtcTime date;
              rtc->getDate(&date);
              event.tag = tags[cnt].serverTag;
              event.readerID = readerID;
              event.eventType = TAG::TAMPER;
              event.date = date;
              events.add(&event, 1);
              tags[cnt].tamperCnt = TAMPER_RESEND;
            } else {
              tags[cnt].tamperCnt--;
            }
          } else {
            tags[cnt].tamperCnt = 0;
          }
          if (tags[cnt].isOutRange()) {
            XREADER::sEvent02 event;
            sRtcTime date;
            rtc->getDate(&date);
            event.tag = tags[cnt].serverTag;
            event.readerID = readerID;
            event.eventType = TAG::OUT_RANGE;
            event.date = date;
            events.add(&event, 1);
            tags[cnt].setUnused();
          }
          if (tags[cnt].startedMoving()) {
            XREADER::sEvent02 event;
            sRtcTime date;
            rtc->getDate(&date);
            event.tag = tags[cnt].serverTag;
            event.readerID = readerID;
            event.eventType = TAG::MOVEMENT_CHANGED;
            event.date = date;
            events.add(&event, 1);
          }
        } else if (tags[cnt].state == TAG::NEW) {
          if (tags[cnt].isInRange()) {
            sRtcTime date;
            XREADER::sEvent02 event;
            rtc->getDate(&date);
            event.tag = tags[cnt].serverTag;
            event.readerID = readerID;
            event.eventType = TAG::IN_RANGE;
            event.date = date;
            events.add(&event, 1);
            tags[cnt].state = TAG::VISIBLE;
          }
        }
      }
    }
  }
}
/****************************************************************************************/
