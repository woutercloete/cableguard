#ifndef DS3231_H
#define DS3231_H
/****************************************************************************************/
#include "types.h"
#include "i2c.h"
/****************************************************************************************/
namespace DS3231 {
  typedef struct {
      u16 year;
      u08 mon;
      u08 day;
      u08 hour;
      u08 min;
      u08 sec;
  } sTimeDate;
  class Crtc {
    private:
      u08 rtc_id;
      Ci2c* i2c;
    public:
      Crtc(Ci2c* _i2c, u08 _id);
      bool setDate(u16 year, u08 mon, u08 day, u08 wday, u08 hour, u08 min);
      bool getDate(u16* year, u08* mon, u08* day, u08 *wday, u08* hour, u08* min);
      bool getDate(u16* year, u08* mon, u08* day, u08 *wday, u08* hour, u08* min,
                   u08* sec);
      char* getTimestamp(void);
  };
}
using namespace DS3231;
#endif
