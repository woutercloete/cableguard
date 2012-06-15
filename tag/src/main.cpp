/****************************************************************************************/
#include "main.h"
/****************************************************************************************/
using namespace I2C;
using namespace LIS3DH;
/****************************************************************************************/
Cpin scl(ePORTC, 5, ePinOut, true);
Cpin sda(ePORTC, 4, ePinOut, true);
Cpin lp1(ePORTD, 5, ePinOut, true);
Cpin lp2(ePORTD, 6, ePinIn, true);
Ci2c motionBus(0xB8, 8, 64);
Clis3dh motion(&motionBus, ePinHigh);
CC1101 cc1101(1, 0);
CHandler handler(&cc1101, 0x0011007C, WDTO_4S);
/****************************************************************************************/
int main(void) {
  sei();
  lp1.setDisable();
  while (1) {
    handler.run();
    if (motion.moved() || lp2.isEnabled()) {
      handler.tag.movement++;
    }
  }
}
/****************************************************************************************/
