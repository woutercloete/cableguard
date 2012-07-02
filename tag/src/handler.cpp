/****************************************************************************************/
#include <avr/sleep.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <util/atomic.h>
/****************************************************************************************/
#include "handler.h"
#include "cc1101.h"
#include "pin.h"
/****************************************************************************************/
#define PSU_2V1()     cbi(PORTC,3) //
#define PSU_BYPASS()  sbi(PORTC,3) // Pulling this pin to low forces the device into ultra low power bypass mode
#undef RANDOM_MAX
#define RANDOM_MAX 2048
#define CCA_WAIT_COUNT  10
#define TX_INTERVAL 4
#define LIFE_CNT_INTERVAL (900) //900 TX PACKETS IN AN HOUR --> 3600 SECS in HOUR / 4 IN HOURS -> 365*5years*24HOURS
/****************************************************************************************/
Cpin psuBypass(ePORTC, 3, ePinIn, false);
/****************************************************************************************/
void CHandler::run(void) {
	switch (state) {
	case SLEEP:
		cc1101->powerdown();
		//PSU_BYPASS(); //TPS62730 in Bypass mode lowest Iq
		psuBypass.setEnable();
		WatchdogInit(txinterval);
		//BIT_SET_LO(PORTD, 5);
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_enable();
		sleep_mode();
		//======================================================================
		//    SLEEP
		//======================================================================
		sleep_disable();
		//BIT_SET_HI(PORTD, 5);
		wdt_disable();
		// enable WDT in reset mode
		wdt_enable(WDTO_2S);
		wdt_reset();
		psuBypass.setDisable();
		//PSU_2V1(); // Use 2.1 V for transmission to keep current down
		cc1101->wakeUp();
		state = TRANSMIT;
		break;
	case TRANSMIT:
		txSeqNr++;
		if (txSeqNr >= 900) { //900 TX PACKETS IN AN HOUR
			txSeqNr = 0;
			tag.lifecnt++;
		}
		tag.count = txSeqNr;
		//======================================================================
		//    TRANSMIT
		//======================================================================
		wdt_reset();
		// If transmit does not finish in 500ms a System reset is issued.
		cc1101->transmit((u08*) &tag, sizeof(TAG::sRfTag), eCC1100_TX_TYPE_CCA);
//		pktSize = (pktSize + 1) % 10;
//		cc1101->transmit(testDat, pktSize, eCC1100_TX_TYPE_CCA);
		wdt_disable();
		state = SLEEP;
		break;
	}
}
/****************************************************************************************
 * Watchdog timer in order to exit (interrupt)
 WDTO_15MS WDTO_30MS WDTO_60MS  WDTO_120MS WDTO_250MS WDTO_500MS WDTO_1S = 1 s WDTO_2S = 2 s  WDTO_4S = 4 s  WDTO_8S = 8 s
 ****************************************************************************************/
void CHandler::WatchdogInit(u08 time) {
	wdt_enable(time);
	cbi(MCUSR, WDRF);
	//To clear WDE, WDRF must be cleared first.
	WDTCSR |= (1 << WDCE) | (0 << WDE); // Timed sequences for changing WDE - uP ISR wake up - no reset
	WDTCSR |= _BV(WDIE); // Enable Watchdog interrupt
}
/****************************************************************************************/
u08 CHandler::test() {
	u08 cca, cca_cnt, mode;
	cca = 0;
	cca_cnt = 0;

	BIT_SET_LO(PORTD, 5);
	_delay_us(10);
	BIT_SET_HI(PORTD, 5);

	mode = cc1101->readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1F;
	if (mode == MARC_STATE_TXFIFO_UV) {
		cc1101->cmdStrobe(CC1101_SFTX);
	}
	if (mode == MARC_STATE_RXFIFO_OV) {
		cc1101->cmdStrobe(CC1101_SFRX);
	}

	// ================= wait for IDLE MODE ==================
	if ((mode == MARC_STATE_IDLE || mode == MARC_STATE_RX)) {
		cca_cnt = 0;
		// ================= SET RX MODE==========================
		cc1101->cmdStrobe(CC1101_SRX);
		// =================== wait 800us for RX MODE ============
		do {
			mode = cc1101->readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER)
					& 0x1F;
			cca_cnt++;
			_delay_us(10); // IDLE TO RX takes 800us page 54 in datasheet
		} while ((mode != MARC_STATE_RX) && (cca_cnt < 200));
		if (cca_cnt >= 200) {
			cc1101->cmdStrobe(CC1101_SIDLE); // FORCE IDLE MODE
			return false;
		}
		//================= Send Packet to FIFO ==================
		cc1101->writeReg(CC1101_TXFIFO, sizeof(TAG::sRfTag));
		cc1101->writeBurstReg(CC1101_TXFIFO, (u08*) &tag, sizeof(TAG::sRfTag));
		mode = cc1101->readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1F;
		cc1101->rssiValidWait();
		cc1101->cmdStrobe(CC1101_STX);
		while (BIT_IS_HI(READ_GD0,PIN_GD0))
			;
		// ================= Wait 725 us for IDLE MODE ===========
		cca_cnt = 0;
		do {
			mode = cc1101->readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER)
					& 0x1F;
			_delay_us(10); // TX to IDle with calibration - 725 us
		} while ((mode != MARC_STATE_IDLE));
		return true;
	}
	return false;
}
/****************************************************************************************/
// Watchdog timer in order to exit (interrupt)
/****************************************************************************************/

ISR(WDT_vect) {
}

