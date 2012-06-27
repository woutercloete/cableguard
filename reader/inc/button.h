#ifndef BUTTON_H_
#define BUTTON_H_

#include <avr/io.h>
#include "avrlibtypes.h"

typedef enum {
	ePORTA = 1, ePORTB = 2, ePORTC = 3, ePORTD = 4
} ePORT;

typedef enum {
	eBtnIn = 0, eBtnOut = 1, eBtnDebounce = 2
} eBtnState;

class Cbutton {
	bool cntDebounce;
	u08 port;
	u08 pin;
	u08 pinAdr;
	u08 ddrAdr;
	u08 portAdr;
	u08 pinNumber;
	eBtnState state;
	bool pressed;
public:
	Cbutton(ePORT portNumber, u08 pinNumber, bool pullup);
	void service(void);
	bool isIn(void);
	bool isOut(void);
	bool wasPressed(void);
};

#endif
