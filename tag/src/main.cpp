/****************************************************************************************/
#include "main.h"
/****************************************************************************************/
using namespace I2C;
using namespace LIS3DH;
/****************************************************************************************/
const u08 TX_INTERVAL_SLOW = WDTO_4S;
const u08 TX_INTERVAL_FAST = WDTO_1S;
/****************************************************************************************/
Cpin scl(ePORTC, 5, ePinOut, true);
Cpin sda(ePORTC, 4, ePinOut, true);
Cpin lp1(ePORTD, 5, ePinOut, true);
Cpin lp2(ePORTD, 6, ePinIn, true);
Ci2c motionBus(0xB8, 8, 64);
Clis3dh motion(&motionBus, ePinHigh, 3);
CC1101 cc1101(1, 0);
CHandler handler(&cc1101, 0x0011008F, TX_INTERVAL_SLOW);
u08 reg, cntTx;
/****************************************************************************************/
int main(void) {
	sei();
	lp1.setDisable();
	while (1) {
		handler.run();
		if (motion.moved()) {
			handler.tag.movement++;
			cntTx = 10;
		}
		if (lp2.isEnabled()) {
			handler.tag.tamper = 1;
			cntTx = 10;
		}
		if (cntTx > 0) {
			handler.setTxInterval(TX_INTERVAL_FAST);
			cntTx--;
		} else {
			handler.setTxInterval(TX_INTERVAL_SLOW);
			handler.tag.tamper = 0;
		}
	}
}
/****************************************************************************************/
