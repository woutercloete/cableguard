#ifndef PWM_H_
#define PWM_H_

#include <avr/io.h>
#include "avrlibtypes.h"
#include "counter.h"
#include "pin.h"

typedef enum {
	OCR_A, //Output Compare A
	OCR_B
//Output Compare B
} eChannel;

typedef enum {
	NON_INVERTING, INVERTING
} ePolarity;

class CPWM: public CCounter, public Cpin {
	u16 duty;
	u16 period;
	eChannel channel;
public:
	CPWM(u08 devNum, u08 prescale, u16 period, u08 dutyCycle, ePORT portNumber,
			u08 pinNumber, bool pullup, eChannel channel);
	void setPolarity(ePolarity pol);
	void stop(ePinState state);
	void start(ePolarity pol);
	void setDutyCycle(u16 dutyCycle);
};

#endif
