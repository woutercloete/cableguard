#include <avr/interrupt.h>
#include "Ctimer.h"
#include "avrlibdefs.h"

static Ctimer* timer[3];

Ctimer::Ctimer(u08 devNum, u08 prescale, eTimerMode mode) :
  CCounter(devNum, prescale) {
  //Enable the overflow interrupt.
  sbi(_MMIO_BYTE(this->timskn), TOIE0);
  timer[devNum] = this;
  this->mode = mode;
  _MMIO_BYTE(this->ocrna) = 255;
  handler = 0;
}

// Set the timer mode to CTC mode.
// In this mode the timer will overflow at the count value.
void Ctimer::setModeCTC(u08 count) {
  _MMIO_BYTE(this->ocrna) = count;
  cbi(_MMIO_BYTE(this->tccrna), WGM10);
  sbi(_MMIO_BYTE(this->tccrna), WGM11);
  cbi(_MMIO_BYTE(this->tccrnb), WGM12);
}

void Ctimer::disableInterrupt(eTimerIntr intr) {
  switch (intr) {
    case OVERFLOW:
      cbi(_MMIO_BYTE(this->timskn), TOIE0);
      break;
    case COMPAREA:
      cbi(_MMIO_BYTE(this->timskn), OCIE0);
      break;
    case COMPAREB:
      cbi(_MMIO_BYTE(this->timskn), OCIE0);
      break;
  }
}

void Ctimer::enableInterrupt(eTimerIntr intr) {
  switch (intr) {
    case OVERFLOW:
      sbi(_MMIO_BYTE(this->timskn), TOIE0);
      break;
    case COMPAREA:
      sbi(_MMIO_BYTE(this->timskn), OCIE0);
      break;
    case COMPAREB:
      sbi(_MMIO_BYTE(this->timskn), OCIE0);
      break;
  }
}

void Ctimer::attach(void(*userHandler)(void)) {
  handler = userHandler;
}

// Start the timer
// time: The time in ticks when the timer must execute the handler.
// If the time is set to zero then the handler will be executed every time the timer overflows.
void Ctimer::start(u32 time) {
  this->time = time;
  count = 0;
  _MMIO_BYTE(this->tcntn) = 0;
  CCounter::start();
}

void Ctimer::start(u32 time, void(*userHandler)(void)) {
  this->time = time;
  count = 0;
  handler = userHandler;
  _MMIO_BYTE(this->tcntn) = 0;
  CCounter::start();
}

void Ctimer::service(void) {
  count += _MMIO_BYTE(this->ocrna);
  if (count >= time) {
    if (handler != 0) {
      count = 0;
      handler();
    }
    if (mode == SINGLE) {
      CCounter::stop();
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

#if 0
SIGNAL(SIG_OVERFLOW2)
{
  timer[2]->service();
}
#endif

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

