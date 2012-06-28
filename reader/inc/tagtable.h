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
#include "Tfifo.h"
#include "payload.h"
#include "MCP7940x.h"
#include "scheduler.h"
#include "cc1101.h"
#include "storage.h"
/****************************************************************************************/
namespace TAG_TABLE {
  const u08 MAX_NUM_TAGS = 8;
  const u08 MAX_NUM_EVENTS = 8;
  const u16 SERVICE_PERIOD = 1; // Service period in seconds
  const u16 SERVICE_TICKS = SERVICE_PERIOD / TICK_BASE;
  const u16 OUT_RANGE_TIMEOUT = 10; // Seconds before tag is considered out of rssiThreshold
  const u16 OUT_RANGE_TICKS = OUT_RANGE_TIMEOUT/SERVICE_PERIOD;
}
/****************************************************************************************/
using namespace TAG;
using namespace TAG_TABLE;
/****************************************************************************************/
class Ctagtable {
  private:
    Crtc* rtc;
    u32 readerID;
    u32 lostCnt;
    Csignal signal;
    CC1101* cc1101;
    Cstorage* store;
    u08 cntFilter;
  public:
    Tfifo<XREADER::sEvent02> events;
    Ctagtable(Crtc* _rtc, u32 _readerID, CC1101* _cc1101, Cstorage* _store) {
      this->store = _store;
      this->cc1101 = _cc1101;
      events.setBufSize(MAX_NUM_EVENTS);
      readerID = _readerID;
      lostCnt = 0;
      rtc = _rtc;
      signal.setPeriod(TAG_TABLE::SERVICE_TICKS);
      scheduler.attach(&signal);
    }
    bool add(C1101::sRadioPacket* radioPacket);
    void service(void);
};
/****************************************************************************************/
#endif

