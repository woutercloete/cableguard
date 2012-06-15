#ifndef CRTC_H
#define CRTC_H

#include "avrlibtypes.h"             // include our global settings
//#include "Irtc.h"
#include "CI2c.h"

//using namespace IRTC;
using namespace I2C;
typedef struct _sRtcTime {
	u16 year;
	u08 mon;
	u08 day;
	u08 hour;
	u08 min;
	u08 sec;
} sRtcTime;

class Crtc { //: public Irtc {
  private:
    u08 rtc_id;
    CI2C* i2c;
  public:
    Crtc(CI2C* _i2c,u08 _id);
    void start(void);
    bool setDate(u16 year, u08 mon, u08 day, u08 hour, u08 min);
    bool setDate(sRtcTime* time);
    bool getMinutesFromEpoch(u32* minutes);
    bool getSeconds(u32* seconds);
    bool getDate(u16* year, u08* mon, u08* day, u08* hour, u08* min);
    bool getDate(u16* year, u08* mon, u08* day, u08* hour, u08* min, u08* sec);
    bool getDate(sRtcTime* time);
};

#endif
