#include "counter.h"
#include "avrlibdefs.h"

u08 error;

CCounter::CCounter(u08 devNum, u08 prescale) {
	this->prescale = prescale;
	this->devNum = devNum;
	switch (devNum) {
	case 0:
		this->tccrna = 0x53;
		this->tccrnb = 0x53;
		this->tcntn = 0x52;
		this->ocrna = 0x51;
		this->ocrnb = 0x51;
		this->icrn = (u16)&error;
		this->timskn = 0x57;
		break;
	case 1: //Must change address pointers for SFR's
		this->tccrna = 0x4F;
		this->tccrnb = 0x4E;
		this->tcntn = 0x4C;
		this->ocrna = 0x4A;
		this->ocrnb = 0x48;
		this->icrn = 0x46;
		this->timskn = 0x6F;
		// Timer/counter one is 16-bit so set up the high byte of TCNTN
		_MMIO_BYTE(this->tcntn+1) = 0;
		break;
	case 2:
		this->ocrna = 0xB3;
		this->ocrnb = 0xB4;
		this->icrn = (u16)&error;
		this->tccrna = 0xB0;
		this->tccrnb = 0xB1;
		this->tcntn = 0xB2;
		this->timskn = 0x70;
		break;
	case 3:
		this->ocrna = 0x0;
		this->ocrnb = 0x0;
		this->icrn = 0x0;
		this->tccrna = 0x0;
		this->tccrnb = 0x0;
		this->tcntn = 0x0;
		this->timskn = 0x0;
		break;
	}
	_MMIO_BYTE(this->tcntn) = 0;
}

void CCounter::start(void) {
	_MMIO_BYTE(this->tccrnb) = ((_MMIO_BYTE(this->tccrnb) & ~COUNTER_PRESCALE_MASK)
			| prescale);
}

void CCounter::stop(void) {
	_MMIO_BYTE(this->tccrnb) = ((_MMIO_BYTE(this->tccrnb) & ~COUNTER_PRESCALE_MASK)
			| COUNTER_CLK_STOP);
}
