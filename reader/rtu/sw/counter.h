#ifndef COUNTER_H_
#define COUNTER_H_
/****************************************************************************************/
#include <avr/io.h>
#include "types.h"
/****************************************************************************************/
namespace COUNTER {
  const u08 COUNTER_CLK_STOP = 0x00; ///< Counter Stopped
  const u08 COUNTER_CLK_DIV1 = 0x01; ///< Counter clocked at F_CPU
  const u08 COUNTER_CLK_DIV8 = 0x02; ///< Counter clocked at F_CPU/8
  const u08 COUNTER_CLK_DIV64 = 0x03; ///< Counter clocked at F_CPU/64
  const u08 COUNTER_CLK_DIV256 = 0x04; ///< Counter clocked at F_CPU/256
  const u08 COUNTER_CLK_DIV1024 = 0x05; ///< Counter clocked at F_CPU/1024
  const u08 COUNTER_CLK_T_FALL = 0x06; ///< Counter clocked at T falling edge
  const u08 COUNTER_CLK_T_RISE = 0x07; ///< Counter clocked at T rising edge
  const u08 COUNTER_PRESCALE_MASK = 0x07; ///< Counter Prescaler Bit-Mask
  // For timer/counter 2
  const u08 COUNTER2_CLK_DIV32 = 0x03; ///< Counter 2 clocked at F_CPU/32
  const u08 COUNTER2_CLK_DIV64 = 0x04; ///< Counter 2 clocked at F_CPU/64
  const u08 COUNTER2_CLK_DIV128 = 0x05; ///< Counter 2 clocked at F_CPU/128
  const u08 COUNTER2_CLK_DIV256 = 0x06; ///< Counter 2 clocked at F_CPU/256
  const u08 COUNTER2_CLK_DIV1024 = 0x07; ///< Counter 2 clocked at F_CPU/1024
  class Ccounter {
    protected:
      u08 devNum;
      u08 prescale;
      u08 ocrna;
      u08 ocrnb;
      u08 icrn;
      u08 tccrna;
      u08 tccrnb;
      u08 tcntn;
      u08 timskn;
      u08 error;
    public:
      Ccounter(u08 devNum, u08 prescale) {
        this->prescale = prescale;
        this->devNum = devNum;
        switch (devNum) {
          case 0:
            this->tccrna = 0x44;
            this->tccrnb = 0x45;
            this->tcntn = 0x46;
            this->ocrna = 0x47;
            this->ocrnb = 0x48;
            this->icrn = 0;
            this->timskn = 0x6E;
            break;
          case 1: //Must change address pointers for SFR's
            this->tccrna = 0x80;
            this->tccrnb = 0x81;
            this->tcntn = 0x84;
            this->ocrna = 0x88;
            this->ocrnb = 0x8A;
            this->icrn = 0x86;
            this->timskn = 0x6F;
            // Timer/counter one is 16-bit so set up the high byte of TCNTN
            _MMIO_BYTE(this->tcntn+1) = 0;
            break;
          case 2:
            this->ocrna = 0xB3;
            this->ocrnb = 0xB4;
            this->icrn = (u16) &error;
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
      void start(void) {
        _MMIO_BYTE(this->tccrnb) = ((_MMIO_BYTE(this->tccrnb) & ~COUNTER_PRESCALE_MASK)
            | prescale);
      }
      void stop(void) {
        _MMIO_BYTE(this->tccrnb) = ((_MMIO_BYTE(this->tccrnb) & ~COUNTER_PRESCALE_MASK)
            | COUNTER_CLK_STOP);
      }
  };
}
using namespace COUNTER;
#endif
