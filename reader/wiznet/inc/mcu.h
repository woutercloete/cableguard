/*
 * mcu.h
 *
 *  Created on: 20 Mar 2012
 *      Author: wouter
 */

#ifndef MCU_H_
#define MCU_H_

#include <util/atomic.h>

#define ATMEGA128_0WAIT   0
#define ATMEGA128_1WAIT   1
#define ATMEGA128_2WAIT   2
#define ATMEGA128_3WAIT   3
#define ATMEGA128_NUM_WAIT  ATMEGA128_1WAIT

class Cmcu {
  public:
    u32 errorState;
    Cmcu() {
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
#if (ATMEGA128_NUM_WAIT == ATMEGA128_0WAIT)
        MCUCR = 0xc0;
        XMCRA = 0x40;
#elif (ATMEGA128_NUM_WAIT == ATMEGA128_1WAIT)
        MCUCR = 0xc0; // Enable external ram
        XMCRA = 0x40; // External Memory Control Register A :
        // Low sector   : 0x1100 ~ 0x7FFF
        // Upper sector : 0x8000 ~ 0xFFFF
#elif (ATMEGA128_NUM_WAIT == ATMEGA128_2WAIT )
        MCUCR = 0xc0;
        XMCRA = 0x42;
#elif (ATMEGA128_NUM_WAIT == ATMEGA128_3WAIT)
        MCUCR = 0xc0;
        XMCRA = 0x42;
#else
#error "unknown atmega128 number wait type"
#endif
        errorState = 0;
      }
    }
};

#endif /* MCU_H_ */
