//****************************************************************************************
// A simple class to setup and control an AVR port pin.
//
//   Created : 08 Apr 2011
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
//****************************************************************************************
#ifndef PIN_H_
#define PIN_H_
//****************************************************************************************
#include "types.h"
//****************************************************************************************
#define PIN_ADR     (this->pinAdr)
#define DDR_ADR     (this->pinAdr+1)
#define PORT_ADR    (this->pinAdr+2)
//****************************************************************************************
namespace PIN {
  typedef enum _ePORT {
    ePORTA = 1,
      ePORTB = 2,
      ePORTC = 3,
      ePORTD = 4,
      ePORTE = 5,
      ePORTF = 6,
      ePORTG = 7,
      ePORTH = 8,
      ePORTJ = 9,
      ePORTK = 10,
      ePORTL = 11
  } ePort;
//****************************************************************************************
  typedef enum {
    LOW = 0, HIGH = 1
  } ePinState;
//****************************************************************************************
  typedef enum {
    IN = 0, OUT = 1
  } ePinDir;
//****************************************************************************************
  class Cpin {
      bool activeLow;
      u08 pin;
      u16 pinAdr;
      voidFuncPtr func;
      void* object;
    public:
      Cpin(ePort portNumber, u08 pinNumber, ePinDir dir, bool pullup = false,
           bool activeLow = false);
      void attach(voidFuncPtr func, void* object);
      void isr(void);
      bool enabled(void);
      bool disabled(void);
      void enable(void);
      void disable(void);
      void toggle(void);
      void enablePullup() {
        (*(volatile u08*) (PORT_ADR)) |= (1 << pin);
      }
      void setDir(ePinDir _dir) {
        if (dir == IN) {
          (*(volatile u08*) (DDR_ADR)) &= ~(1 << pinNumber);
        } else {
          (*(volatile u08*) (DDR_ADR)) |= (1 << pinNumber);
        }
      }
  };
}
//****************************************************************************************
using namespace PIN;
#endif
