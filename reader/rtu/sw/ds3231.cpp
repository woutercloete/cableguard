#include <stdio.h>
#include <string.h>
#include <stdlib.h>		//
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support
#include "ds3231.h"

#define BCD_TO_BIN(x) (((x) & 0x0F) + ((x) >> 4) * 10)
#define BIN_TO_BCD(x) ((((x) / 10) << 4) + (x) % 10)

u32 mktime(u16 year, u08 mon, u08 day, u08 hour, u08 min);

/*==========================================================================
 FUNCTION:DS3231 driver
 PURPOSE:
 ---------------------------------------------------------------------------*/
Crtc::Crtc(Ci2c* _i2c, u08 _id) {
  rtc_id = _id;
  i2c = _i2c;
}

/*==========================================================================
 FUNCTION:DS3231 driver
 PURPOSE:
 ---------------------------------------------------------------------------*/
bool Crtc::getDate(u16* year, u08* mon, u08* day, u08 *wday, u08* hour, u08* min) {
  u08 buf[0x12];
  // send address
  buf[0] = 0x00;
  i2c->tx(1, buf);
  // get the data
  i2c->rx(7, buf);

  //datetime.sec = BCD_TO_BIN (buf[0] & 0x7F);
  *min = BCD_TO_BIN (buf[1] & 0x7F);
  *hour = BCD_TO_BIN (buf[2] & 0x3F);
  //*wday = (buf[3]) & 0x7; //1--7
  *day = BCD_TO_BIN (buf[4] & 0x3F); //01--31
  *mon = BCD_TO_BIN (buf[5] & 0x1F);
  *year = BCD_TO_BIN (buf[6]) + 2000; /* byte*/
  return true;
}

bool Crtc::getDate(u16* year, u08* mon, u08* day, u08 *wday, u08* hour, u08* min,
                   u08* sec) {
  u08 buf[0x12];
  // send address
  buf[0] = 0x00;
  i2c->tx(1, buf);
  // get the data
  i2c->rx(7, buf);

  *sec = BCD_TO_BIN (buf[0] & 0x7F);
  *min = BCD_TO_BIN (buf[1] & 0x7F);
  *hour = BCD_TO_BIN (buf[2] & 0x3F);
  *wday = (buf[3]) & 0x7; //1--7
  *day = BCD_TO_BIN (buf[4] & 0x3F); //01--31
  *mon = BCD_TO_BIN (buf[5] & 0x1F);
  *year = BCD_TO_BIN (buf[6]) + 2000; /* byte*/
  return true;
}

/*==========================================================================
 FUNCTION:DS3231 driver
 PURPOSE:
 ---------------------------------------------------------------------------*/
bool Crtc::setDate(u16 year, u08 mon, u08 day, u08 wday, u08 hour, u08 min) {
  u08 buf[8];
  /* select address 0x00 */
  buf[0] = 0x00;

  buf[1] = (BIN_TO_BCD (0)); //sec
  buf[2] = (BIN_TO_BCD (min)) & 0x7F;
  buf[3] = (BIN_TO_BCD (hour)) & 0x3F; // 00-->23
  buf[4] = wday;
  buf[5] = (BIN_TO_BCD (day)) & 0x3F; //1--31/30
  buf[6] = BIN_TO_BCD (mon) & 0x1F; //1--12
  buf[7] = BIN_TO_BCD (year-2000); //00 -- 99
  i2c->tx(8, buf);
  return true;
}

char* Crtc::getTimestamp(void) {
  char* aux = "\0                                                ";
  u16 year;
  u08 month;
  u08 date;
  u08 day;
  u08 hour;
  u08 minute;
  u08 second;

  getDate(&year, &month, &date, &day, &hour, &minute, &second);
  sprintf(aux, "%d/%02d/%02d %02d:%02d:%02d", year, month, date, hour, minute, second);
  return aux;
}

