/****************************************************************************************/
// The Ctag class contains an RFID tag
//
//   Created : 15 March 2012
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
/****************************************************************************************/
#ifndef TAG_H
#define TAG_H
/****************************************************************************************/
#include "avrlibtypes.h"
#include "fir.h"
#include "lis3dh.h"
/****************************************************************************************/
using namespace FIR;
/****************************************************************************************/
const u08 tickThreshold = 20;
const u08 rssiMargin = 5;
/****************************************************************************************/
namespace TAG {
  typedef struct {
      u32 tagID;
      u08 count;
      u16 lifecnt;
      u08 movement;
      u08 tamper;
      sAccel acel;
  } sRfTag;
  /****************************************************************************************/
  typedef struct {
      TAG::sRfTag rfTag;
      u32 rssi;
      u32 lqi;
  } sServerTag02;
  /****************************************************************************************/
  typedef enum {
    UNUSED = 0x1, NEW = 0x2, VISIBLE = 0x3, FADING = 0x4, INVISIBLE = 0x5
  } eState;
  /****************************************************************************************/
  typedef enum {
    IN_RANGE = 0x1, OUT_RANGE = 0x2, MOVEMENT_CHANGED = 0x3, TAMPER = 0x4
  } eEvent;
/****************************************************************************************/
}
/****************************************************************************************/
class Ctag {
    friend class Ctagtable;
  private:
    u08 cntTick;
    TAG::eState state;
    Cfir rssiFilter;
    u08 rssiIn;
    u08 movementPrev;
    u08 movementPrevPrev;
    bool movingNow;
    bool movingPrev;
    bool stoppedMoving;
  public:
    bool tamper;
    u08 tamperCnt;
    u08 movementNow;
    TAG::sServerTag02 serverTag;
    u16 rssiThreshold;
    u08 rssiOut;
    //bool movementChanged;
    Ctag() {
      state = TAG::UNUSED;
      cntTick = 0;
      rssiThreshold = 90;
      serverTag.rfTag.tagID = 0;
      serverTag.rfTag.count = 0;
      serverTag.rfTag.lifecnt = 0;
      serverTag.rfTag.movement = 0;
      serverTag.rssi = 90;
      rssiFilter.setSize(8);
      rssiFilter.setWeight(0.125, true);
      for (u08 cnt = 0; cnt < 7; cnt++) {
        rssiFilter.setWeight(0.125, false);
      }
    }
    void update(TAG::sServerTag02* _serverTag) {
      movementPrevPrev = movementPrev;
      movementPrev = movementNow;
      movementNow = _serverTag->rfTag.movement;
      movingNow = !(movementPrev == movementNow && movementPrev == movementPrevPrev);
      cntTick = 0;
      serverTag.rssi = rssiOut;
      serverTag.rfTag.movement = movingNow;
      serverTag.rfTag.count = _serverTag->rfTag.count;
      serverTag.rfTag.lifecnt = _serverTag->rfTag.lifecnt;
      serverTag.rfTag.tagID = _serverTag->rfTag.tagID;
      this->rssiIn = _serverTag->rssi;
    }
    bool startedMoving() {
      bool ret = (movingNow && !movingPrev);
      movingPrev = movingNow;
      return (ret);
    }
    bool sopedMoving() {
      bool ret = (!movingNow && movingPrev);
      movingPrev = movingNow;
      return (ret);
    }

    void setRange(u16 _rssiThreshold) {
      this->rssiThreshold = _rssiThreshold;
    }
    void setNew(TAG::sServerTag02* _serverTag, u16 _rssiThreshold) {
      tamper = 0;
      tamperCnt = 0;
      cntTick = 0;
      this->rssiThreshold = _rssiThreshold;
      serverTag.rssi = _serverTag->rssi;
      serverTag.rfTag.movement = _serverTag->rfTag.movement;
      movementNow = _serverTag->rfTag.movement;
      movementPrev = _serverTag->rfTag.movement;
      movementPrevPrev = _serverTag->rfTag.movement;
      movingNow = false;
      movingPrev = false;
      serverTag.rfTag.count = _serverTag->rfTag.count;
      serverTag.rfTag.lifecnt = _serverTag->rfTag.lifecnt;
      serverTag.rfTag.tagID = _serverTag->rfTag.tagID;
      this->rssiIn = _serverTag->rssi;
      this->rssiOut = _serverTag->rssi;
      rssiFilter.fill(_serverTag->rssi);
      state = TAG::NEW;
    }
    void setUnused() {
      state = TAG::UNUSED;
    }
    bool isUsed() {
      return state != TAG::UNUSED;
    }
    void clock() {
      rssiFilter.clock(&rssiIn, &rssiOut);
      cntTick++;
    }
    void setState(TAG::eState state) {
      this->state = state;
    }
    u16 getLife() {
      return serverTag.rfTag.lifecnt;
    }
    u32 getTagID() {
      return serverTag.rfTag.tagID;
    }
    bool getState() {
      return state;
    }
    void resetTick(void) {
      cntTick = 0;
    }
    u32 getID(void) {
      return serverTag.rfTag.tagID;
    }
    bool isOutRange() {
      return ((rssiOut > rssiThreshold + rssiMargin) || (cntTick > tickThreshold));
    }
    bool isInRange() {
      return (rssiOut < rssiThreshold - rssiMargin);
    }
    void service();
};
/****************************************************************************************/
#endif

