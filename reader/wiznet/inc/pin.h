#ifndef PIN_H_
#define PIN_H_

#include "avrlibtypes.h"

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

typedef enum {
  ePinLow = 0,
  ePinHigh = 1
} ePinState;

typedef enum {
  ePinIn = 0,
  ePinOut = 1
} ePinDir;
/****************************************************************************************/
class Cpin {
    u08 pin;
    u16 pinAdr;
    voidFuncPtr func;
    void* object;
  public:
    Cpin(ePORT portNumber, u08 pinNumber, ePinDir dir, bool pullup);
    void attach(voidFuncPtr func, void* object);
    void isr(void);
    bool isHigh(void);
    bool isLow(void);
    void setHigh(void);
    void setLow(void);
    void toggle(void);
};
#endif
