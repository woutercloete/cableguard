#include <stdio.h>
#include <string.h>
#include <stdlib.h>		//
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support
#include "MCP7940x.h"

#define BCD_TO_BIN(x) (((x) & 0x0F) + ((x) >> 4) * 10)
#define BIN_TO_BCD(x) ((((x) / 10) << 4) + (x) % 10)

u32 mktime(u16 year, u08 mon, u08 day, u08 hour, u08 min);

/*==========================================================================
 FUNCTION:DS3231 driver
 PURPOSE:
 ---------------------------------------------------------------------------*/
Crtc::Crtc(CI2C* _i2c, u08 _id) {
	rtc_id = _id << 0x1;
	i2c = _i2c;
}



void Crtc::start(void) {
	u08 buf[0x12];
	// send address
	buf[0] = 0x00;
	i2c->masterSend(rtc_id, 1, buf);
	// Read the seconds
	i2c->masterReceive(rtc_id, 1, buf);
	buf[1] = buf[0] | 0x80;
	buf[0] = 0x00;
	i2c->masterSend(rtc_id, 2, buf);
}
/*==========================================================================
 FUNCTION:DS3231 driver
 PURPOSE:
 ---------------------------------------------------------------------------*/
bool Crtc::getDate(u16* year, u08* mon, u08* day, u08* hour, u08* min) {
	u08 buf[0x12];
	// send address
	buf[0] = 0x00;
	i2c->masterSend(rtc_id, 1, buf);
	// get the data
	i2c->masterReceive(rtc_id, 7, buf);

	//datetime.sec = BCD_TO_BIN (buf[0] & 0x7F);
	*min = BCD_TO_BIN (buf[1] & 0x7F);
	*hour = BCD_TO_BIN (buf[2] & 0x3F);
	//*wday = (buf[3]) & 0x7; //1--7
	*day = BCD_TO_BIN (buf[4] & 0x3F); //01--31
	*mon = BCD_TO_BIN (buf[5] & 0x1F);
	*year = BCD_TO_BIN (buf[6]) + 2000; /* byte*/
	return true;
}

bool Crtc::getDate(u16* year, u08* mon, u08* day, u08* hour, u08* min,
		u08* sec) {
	u08 buf[0x12];
	// send address
	buf[0] = 0x00;
	i2c->masterSend(rtc_id, 1, buf);
	// get the data
	i2c->masterReceive(rtc_id, 7, buf);

	*sec = BCD_TO_BIN (buf[0] & 0x7F);
	*min = BCD_TO_BIN (buf[1] & 0x7F);
	*hour = BCD_TO_BIN (buf[2] & 0x3F);
	//*wday = (buf[3]) & 0x7; //1--7
	*day = BCD_TO_BIN (buf[4] & 0x3F); //01--31
	*mon = BCD_TO_BIN (buf[5] & 0x1F);
	*year = BCD_TO_BIN (buf[6]) + 2000; /* byte*/
	return true;
}

bool Crtc::getDate(sRtcTime* time) {
	u08 buf[0x12];
	// send address
	buf[0] = 0x00;
	i2c->masterSend(rtc_id, 1, buf);
	// get the data
	i2c->masterReceive(rtc_id, 7, buf);

	time->sec = BCD_TO_BIN (buf[0] & 0x7F);
	time->min = BCD_TO_BIN (buf[1] & 0x7F);
	time->hour = BCD_TO_BIN (buf[2] & 0x3F);
	//*wday = (buf[3]) & 0x7; //1--7
	time->day = BCD_TO_BIN (buf[4] & 0x3F); //01--31
	time->mon = BCD_TO_BIN (buf[5] & 0x1F);
	time->year = BCD_TO_BIN (buf[6]) + 2000; /* byte*/
	return true;
}

/*==========================================================================
 FUNCTION:DS3231 driver
 PURPOSE:
 ---------------------------------------------------------------------------*/
bool Crtc::setDate(u16 year, u08 mon, u08 day, u08 hour, u08 min) {
	u08 buf[8];
	/* select address 0x00 */
	buf[0] = 0x00;

	buf[1] = (BIN_TO_BCD (0)) | 0x80; //sec
	buf[2] = (BIN_TO_BCD (min)) & 0x7F;
	buf[3] = (BIN_TO_BCD (hour)) & 0x3F;// 00-->23
	buf[4] = 0xFF; // VBATEN - BIT3 VBAT = BIT4
	buf[5] = (BIN_TO_BCD (day)) & 0x3F; //1--31/30
	buf[6] = BIN_TO_BCD (mon) & 0x1F; //1--12
	buf[7] = BIN_TO_BCD (year-2000); //00 -- 99
	i2c->masterSend(rtc_id, 8, buf);
	return true;
}

bool Crtc::setDate(sRtcTime* time) {
	u08 buf[8];
	/* select address 0x00 */
	buf[0] = 0x00;

	buf[1] = (BIN_TO_BCD (time->sec)) | 0x80; //sec
	buf[2] = (BIN_TO_BCD (time->min)) & 0x7F;
	buf[3] = (BIN_TO_BCD (time->hour)) & 0x3F;// 00-->23
	buf[4] = 7; //1-7 wday
	buf[5] = (BIN_TO_BCD (time->day)) & 0x3F; //1--31/30
	buf[6] = BIN_TO_BCD (time->mon) & 0x1F; //1--12
	buf[7] = BIN_TO_BCD (time->year-2000); //00 -- 99
	i2c->masterSend(rtc_id, 8, buf);
	return true;
}

bool Crtc::getMinutesFromEpoch(u32* minutesFromEpoch) {
	u32 minutes_2008_09_17;
	u32 minutes_now;
	u16 year;
	u08 mon;
	u08 day;
	u08 hour;
	u08 min;
	getDate(&year, &mon, &day, &hour, &min);
	/* calc minutes from 1970 to epoch */
	minutes_2008_09_17 = mktime(2009, 5, 9, 0, 0);
	minutes_now = mktime(year, mon, day, hour, min);
	*minutesFromEpoch = (minutes_now - minutes_2008_09_17);
	return true;
}
//---------------------------------------------------------------------------
bool Crtc::getSeconds(u32* seconds) {
	u32 minutes_2010;
	u32 minutes_now;
	u16 year;
	u08 mon;
	u08 day;
	u08 hour;
	u08 min;
	u08 sec;
	getDate(&year, &mon, &day, &hour, &min, &sec);
	/* calc minutes from 1970 to 2010 */
	minutes_2010 = mktime(2010, 1, 1, 0, 0);
	minutes_now = mktime(year, mon, day, hour, min);
	*seconds = ((minutes_now - minutes_2010) * 60) + sec;
	return true;
}
//---------------------------------------------------------------------------
/*
 * Converts Gregorian date to seconds since 1970-01-01 00:00:00.
 * Assumes input in normal date format, i.e. 1980-12-31 23:59:59
 * => year=1980, mon=12, day=31, hour=23, min=59, sec=59.
 *
 * This algorithm was first published by Gauss.
 *
 * WARNING: this function will overflow on 2106-02-07 06:28:16 on
 * machines were long is 32-bit! (However, as time_t is signed, we
 * will already get problems at other places on 2038-01-19 03:14:08)
 */
#if 0
u32 long mktime(u16 year, u08 mon, u08 day, u08 hour, u08 min) {
	if (0 >= (int) (mon -= 2)) { /* 1..12 -> 11,12,1..10 */
		mon += 12; /* Puts Feb last since it has leap day */
		year -= 1;
	}
	return (((u32) (year / 4 - year / 100 + year / 400 + 367 * mon / 12 + day)
					+ year * 365 - 719499) * 24 + hour) * 60 + min;
}
#endif

u32 mktime(u16 year, u08 mon, u08 day, u08 hour, u08 min) {
	u32 tmp;
	s16 imon;
	imon = (s16) mon;

	if (0 >= (imon -= 2)) { /* 1..12 -> 11,12,1..10 */
		imon += 12; /* Puts Feb last since it has leap day */
		year -= 1;
	}
	tmp = (u32) ((year / 4) - (year / 100) + (year / 400) + ((367 * imon) / 12)
			+ day);
	tmp += ((u32) year * 365) - 719499;
	tmp *= 24;
	tmp += hour;
	tmp *= 60;
	tmp += min;
	return tmp;
}

