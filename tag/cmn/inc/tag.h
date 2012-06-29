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
/****************************************************************************************/
using namespace FIR;
/****************************************************************************************/
namespace TAG {
  typedef struct {
      u32 tagID;
      u08 count;
      u16 lifecnt;
      u08 movement;
      u08 tamper;
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
    IN_RANGE = 0x1, OUT_RANGE = 0x2, MOVEMENT_CHANGED = 0x3
  } eEvent;
/****************************************************************************************/
}
/****************************************************************************************/
class Ctag {
    friend class Ctagtable;
  private:
    TAG::sServerTag02 serverTag;
    u08 cntTick;
    TAG::eState state;
    u16 rssiThreshold;
    Cfir rssiFilter;
    //Cfir movementFilter;
    u08 rssiIn;
    //u08 movIn;
    //u08 movOut;
    //bool oldMoving;
    //u08 movement;
    u08 movementNow;
    u08 movementPrev;
    u08 movementPrevPrev;
    bool movingNow;
    bool movingPrev;
    bool startedMoving;
    bool stoppedMoving;
  public:
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
//      rssiFilter.setWeight(-0.023957294012551478, true);
//      rssiFilter.setWeight(-0.023759522572675367, false);
//      rssiFilter.setWeight(0.152797829818546090, false);
//      rssiFilter.setWeight(0.415345926257756510, false);
//      rssiFilter.setWeight(0.415345926257756510, false);
//      rssiFilter.setWeight(0.152797829818546090, false);
//      rssiFilter.setWeight(-0.023759522572675367, false);
//      rssiFilter.setWeight(-0.023957294012551478, false);
//      movementFilter.setSize(3);
//      movementFilter.setWeight(0.33, true);
//      movementFilter.setWeight(0.33, false);
//      movementFilter.setWeight(0.33, false);
//      movIn = 0;
//      oldMoving = false;
      //movementChanged = false;
    }
    void update(TAG::sServerTag02* _serverTag) {
      movementPrevPrev = movementPrev;
      movementPrev = movementNow;
      movementNow = _serverTag->rfTag.movement;
      movingNow = !(movementPrev == movementNow && movementPrev == movementPrevPrev);
      if (movingNow != movingPrev) {
        if (movingNow) {
          startedMoving = true;
          stoppedMoving = false;
        } else {
          startedMoving = false;
          stoppedMoving = true;
        }
      } else {
        startedMoving = false;
        stoppedMoving = false;
      }
      movingPrev = movingNow;
      cntTick = 0;
      serverTag.rssi = rssiOut;
      serverTag.rfTag.movement = movingNow;
      //this->movIn = _serverTag->rfTag.movement;
      serverTag.rfTag.count = _serverTag->rfTag.count;
      serverTag.rfTag.lifecnt = _serverTag->rfTag.lifecnt;
      serverTag.rfTag.tagID = _serverTag->rfTag.tagID;
      this->rssiIn = _serverTag->rssi;
    }
    void setRange(u16 _rssiThreshold) {
      this->rssiThreshold = _rssiThreshold;
    }
    void setNew(TAG::sServerTag02* _serverTag, u16 _rssiThreshold) {
      cntTick = 0;
      this->rssiThreshold = _rssiThreshold;
      serverTag.rssi = 90;
      serverTag.rfTag.movement = false;
      serverTag.rfTag.count = _serverTag->rfTag.count;
      serverTag.rfTag.lifecnt = _serverTag->rfTag.lifecnt;
      serverTag.rfTag.tagID = _serverTag->rfTag.tagID;
      this->rssiIn = serverTag.rssi;
      this->rssiOut = 90;
      //this->movIn = 0;
      rssiFilter.fill(90);
      //movementFilter.fill(0);
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
      //movementFilter.clock(&movIn, &movOut);
      //movementChanged = (oldMoving != moved);
      //oldMoving = moved;
      cntTick++;
    }
    void setState(TAG::eState state) {
      this->state = state;
    }
    u08 moved() {
//      if (movOut > 127) {
//        return true;
//      }
      return false;
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
//    void setDat(TAG::sRfTag* rfTag, u32 rssi) {
//      this->serverTag.rfTag = *rfTag;
//      this->serverTag.rfTag.movement = oldMoving;
//      this->serverTag.rssi = rssi;
//    }
    void resetTick(void) {
      cntTick = 0;
    }
    u32 getID(void) {
      return serverTag.rfTag.tagID;
    }
    bool isOutRange() {
      return (rssiOut > rssiThreshold + 5);
    }
    bool isInRange() {
      return (rssiOut < rssiThreshold - 5);
    }
    void service();
};
/****************************************************************************************/
#endif

