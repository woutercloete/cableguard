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
#include "avrlibtypes.h"
//****************************************************************************************
#define PIN_ADR     (this->pinAdr)
#define DDR_ADR     (this->pinAdr+1)
#define PORT_ADR    (this->pinAdr+2)
//****************************************************************************************
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
} ePORT;
//****************************************************************************************
typedef enum {
  ePinLow = 0, ePinHigh = 1
} ePinState;
//****************************************************************************************
typedef enum {
  ePinIn = 0, ePinOut = 1
} ePinDir;
//****************************************************************************************
class Cpin {
    bool activeLow;
    u08 pin;
    u16 pinAdr;
    voidFuncPtr func;
    void* object;
  public:
    Cpin(ePORT portNumber, u08 pinNumber, ePinDir dir, bool pullup = false, bool activeLow = false);
    void attach(voidFuncPtr func, void* object);
    void isr(void);
    bool isEnabled(void);
    bool isDisabled(void);
    void setEnable(void);
    void setDisable(void);
    void toggle(void);
};
//****************************************************************************************
#endif
