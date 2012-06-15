#include <avr/interrupt.h>
#include "timer.h"
#include "types.h"

static CTimer* timer[3];

CTimer::CTimer(u08 devNum, u08 prescale, eTimerMode mode) :
	Ccounter(devNum, prescale) {
	//Enable the overflow interrupt.
	setbit(_MMIO_BYTE(this->timskn), TOIE1);
	timer[devNum] = this;
	this->mode = mode;
	_MMIO_BYTE(this->ocrna) = 255;
	handler = 0;
}

// Set the timer mode to CTC mode.
// In this mode the timer will overflow at the count value.
void CTimer::setModeCTC(u08 count) {
	_MMIO_BYTE(this->ocrna) = count;
	clearbit(_MMIO_BYTE(this->tccrna), WGM10);
	setbit(_MMIO_BYTE(this->tccrna), WGM11);
	clearbit(_MMIO_BYTE(this->tccrnb), WGM12);
}

void CTimer::disableInterrupt(eTimerIntr intr) {
	switch (intr) {
	case OVERFLOW:
		clearbit(_MMIO_BYTE(this->timskn), TOIE1);
		break;
	case COMPAREA:
		clearbit(_MMIO_BYTE(this->timskn), OCIE1A);
		break;
	case COMPAREB:
		clearbit(_MMIO_BYTE(this->timskn), OCIE1B);
		break;
	}
}

void CTimer::enableInterrupt(eTimerIntr intr) {
	switch (intr) {
	case OVERFLOW:
		setbit(_MMIO_BYTE(this->timskn), TOIE1);
		break;
	case COMPAREA:
		setbit(_MMIO_BYTE(this->timskn), OCIE1A);
		break;
	case COMPAREB:
		setbit(_MMIO_BYTE(this->timskn), OCIE1B);
		break;
	}
}

void CTimer::attach(void(*userHandler)(void)) {
	handler = userHandler;
}

// Start the timer
// time: The time in ticks when the timer must execute the handler.
// If the time is set to zero then the handler will be executed every time the timer overflows.
void CTimer::start(u32 time) {
	this->time = time;
	count = 0;
	_MMIO_BYTE(this->tcntn) = 0;
	Ccounter::start();
}

void CTimer::start(u32 time, void(*userHandler)(void)) {
	this->time = time;
	count = 0;
	handler = userHandler;
	_MMIO_BYTE(this->tcntn) = 0;
	Ccounter::start();
}

void CTimer::service(void) {
	count += _MMIO_BYTE(this->ocrna);
	if (count > time) {
		if (handler != 0)
			handler();
		if (mode == SINGLE) {
			Ccounter::stop();
		} else {
			count = 0;
		}
	}
}

SIGNAL(SIG_OVERFLOW0)
{
	timer[0]->service();
}

SIGNAL(SIG_OVERFLOW1)
{
	timer[1]->service();
}

SIGNAL(SIG_OVERFLOW2)
{
	timer[2]->service();
}

SIGNAL(SIG_OUTPUT_COMPARE0A)
{
	timer[0]->service();
}

SIGNAL(SIG_OUTPUT_COMPARE1A)
{
	timer[1]->service();
}

SIGNAL(SIG_OUTPUT_COMPARE2A)
{
	timer[2]->service();
}


