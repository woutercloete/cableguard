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
#include "pin.h"
//****************************************************************************************
Cpin::Cpin(ePort portNumber, u08 pinNumber, ePinDir dir, bool pullup, bool activeLow) {
  switch (portNumber) {
    case ePORTA:
      this->pinAdr = 0x20;
      break;
    case ePORTB:
      this->pinAdr = 0x23;
      break;
    case ePORTC:
      this->pinAdr = 0x26;
      break;
    case ePORTD:
      this->pinAdr = 0x29;
      break;
    case ePORTE:
      this->pinAdr = 0x2C;
      break;
    case ePORTF:
      this->pinAdr = 0x2F;
      break;
    case ePORTG:
      this->pinAdr = 0x32;
      break;
    case ePORTH:
      this->pinAdr = 0x100;
      break;
    case ePORTJ:
      this->pinAdr = 0x103;
      break;
    case ePORTK:
      this->pinAdr = 0x106;
      break;
    case ePORTL:
      this->pinAdr = 0x109;
      break;
  }
  this->pin = pinNumber;
  this->activeLow = activeLow;
  if (dir == IN) {
    // Set the pin as an input by clearing the bit in the direction register.
    (*(volatile u08*) (DDR_ADR)) &= ~(1 << pinNumber);
  } else {
    // Set the pin as an output by setting the bit in the direction register.
    (*(volatile u08*) (DDR_ADR)) |= (1 << pinNumber);
  }
  // If the pullup needs to be on configure it.
  if (pullup) {
    (*(volatile u08*) (PORT_ADR)) |= (1 << pinNumber);
  } else {
    (*(volatile u08*) (PORT_ADR)) &= ~(1 << pinNumber);
  }
}
//****************************************************************************************
void Cpin::attach(voidFuncPtr func, void* object) {
  this->func = func;
  this->object = object;
}
//****************************************************************************************
bool Cpin::enabled(void) {
  if (activeLow) {
    return ((((*(volatile u08*) (this->pinAdr)) >> pin) & 0x1) == 0x0);
  } else {
    return ((((*(volatile u08*) (this->pinAdr)) >> pin) & 0x1) == 0x1);
  }
}
//****************************************************************************************
bool Cpin::disabled(void) {
  if (activeLow) {
    return ((((*(volatile u08*) (this->pinAdr)) >> pin) & 0x1) == 0x1);
  } else {
    return ((((*(volatile u08*) (this->pinAdr)) >> pin) & 0x1) == 0x0);
  }

}
//****************************************************************************************
void Cpin::enable(void) {
  if (!activeLow) {
    *(volatile u08*) (PORT_ADR) |= (0x1 << pin);
  } else {
    *(volatile u08*) (PORT_ADR) &= ~(0x1 << pin);
  }
}
//****************************************************************************************
void Cpin::disable(void) {
  if (!activeLow) {
    *(volatile u08*) (PORT_ADR) &= ~(0x1 << pin);
  } else {
    *(volatile u08*) (PORT_ADR) |= (0x1 << pin);
  }
}
//****************************************************************************************
void Cpin::toggle(void) {
  if (disabled()) {
    enable();
  } else {
    disable();
  }
}
//****************************************************************************************
