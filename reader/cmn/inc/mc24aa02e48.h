//****************************************************************************************
// The interface to the 24AA02E48 memory chip from Microchip.
// http://ww1.microchip.com/downloads/en/DeviceDoc/22124A.pdf
//
//   Created : 2 May 2012
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
//****************************************************************************************
#ifndef MC24AA02E48_H_
#define MC24AA02E48_H_
//****************************************************************************************
#include "i2c.h"
//****************************************************************************************
typedef struct {
    u08 ID[6];
} sId;
//****************************************************************************************
#define DEV_ADR       (u08)(0x50)
#define ID_BASE_ADR   0xFA
//****************************************************************************************
using namespace I2C;
//****************************************************************************************
class Cmc24aa02e48 {
    Ci2c* i2c;
  public:
    Cmc24aa02e48(Ci2c* i2c) {
      this->i2c = i2c;
      i2c->setBitrate(100);
      i2c->setDevAdr(DEV_ADR);
    }
    void readID(sId* id) {
      u08 adr = ID_BASE_ADR;
      // Write the adr
      i2c->tx(1, &adr);
      i2c->rx(6, (u08*) id);
    }
};
//****************************************************************************************
#endif /* MC24AA02E48_H_ */
