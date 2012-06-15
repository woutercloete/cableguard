/* A simple class to setup and control an AVR port.
 *
 * Created on: 18 August 2011
 * Author: Wouter Cloete for Firmlogik (Pty) Ltd
 ****************************************************************************************/
#ifndef PORT_H_
#define PORT_H_
/****************************************************************************************/
#include "avrlibtypes.h"
/****************************************************************************************/
#define PIN_ADR			(this->pinAdr)
#define DDR_ADR			(this->pinAdr+1)
#define PORT_ADR		(this->pinAdr+2)
/****************************************************************************************/
typedef enum _ePORT {
	ePORTA = 1,
	ePORTB = 2,
	ePORTC = 3,
	ePORTD = 4,
	ePORTE = 5,
	ePORTF = 6,
	ePORTG = 7,
	ePORTH = 8,
	ePORTI = 9,
	ePORTJ = 10,
	ePORTK = 11,
	ePORTL = 12
} ePORT;
typedef enum {
	ePinLow = 0, ePinHigh = 1
} ePinState;
typedef enum {
	ePinIn = 0, ePinOut = 1
} ePinDir;
typedef enum {
	ePortNormEndian = 0, ePortSwapEndian = 1
} ePortEndian;
/****************************************************************************************/
class Cport {
	u16 pinAdr;
	bool swapFlag;
public:
	Cport(ePORT portNumber, ePinDir dir, bool pullup, bool swap);
	void write(u08 dat);
	u08 read(void);
};
/****************************************************************************************/
#endif
