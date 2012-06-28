#ifndef HANDLER_H
#define HANDLER_H
/****************************************************************************************/
#include "cc1101.h"
#include "tag.h"
/****************************************************************************************/
typedef enum {
	SLEEP = 1, TRANSMIT = 2
} eState;
/****************************************************************************************/
class CHandler {
	CC1101 *cc1101;
	u08 batteryCnt;
	u16 txSeqNr;
	u08 txinterval;
	eState state;
	void WatchdogInit(u08 time);
	u08 test(void);
public:
	TAG::sRfTag tag;
	CHandler(CC1101* _cc1101, u32 _tagID, u08 _txinterval) {
		tag.tagID = _tagID;
		tag.lifecnt = 0;
		tag.movement = 0xFF;
		tag.count = 0;
		state = SLEEP;
		txSeqNr = 0;
		txinterval = _txinterval;
		cbi(DDRB, 6);
		// GD0 PIN
		sbi(DDRC, 1);
		// PSU PIN
		sbi(DDRD, 5);
		// Test PIN
	};
	void run(void);
};
/****************************************************************************************/
#endif
