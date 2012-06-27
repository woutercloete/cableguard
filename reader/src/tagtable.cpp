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
/****************************************************************************************/
#include "tagtable.h"
/****************************************************************************************/
using namespace C1101;
/****************************************************************************************/
Ctag tags[MAX_NUM_TAGS];
/****************************************************************************************/
u32 convDB(u08 rssi);
/****************************************************************************************/
bool Ctagtable::add(C1101::sRadioPacket* radioPacket) {
  sServerTag02 serverTag;
  u08 cnt;
  // Check if tag is not already in tags
  for (cnt = 0; cnt < MAX_NUM_TAGS; cnt++) {
    if (tags[cnt].isUsed()) {
      if (radioPacket->tag.tagID == tags[cnt].getTagID()) {
        // Update tag
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
      serverTag.rssi = radioPacket->rssi;
      serverTag.lqi = radioPacket->lqi;
      serverTag.rfTag = radioPacket->tag;
      tags[cnt].setNew(&serverTag, store->getRSSIThreshold());
      return true;
    }
  }
  return true;
}
/****************************************************************************************/
void Ctagtable::service(void) {
  if (signal.isSet()) {
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
