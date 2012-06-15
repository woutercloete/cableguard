//****************************************************************************************
// Generic I2C interface.
//
//   Created : 2 May 2012
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
//****************************************************************************************
#include <avr/io.h>
#include <avr/interrupt.h>
//****************************************************************************************
#include "i2c.h"
//****************************************************************************************
using namespace I2C;
//****************************************************************************************
eState Ci2c::mode;
Ci2c* Ci2c::activeDevice;
//****************************************************************************************
SIGNAL(TWI_vect) {
  if (Ci2c::activeDevice != 0) {
    Ci2c::activeDevice->service();
  }
}
//****************************************************************************************
