/*
 * CDisplay.cpp
 *
 *  Created on: 08 Mar 2009
 *      Author: Wouter
 */

#include <string.h>

#include "CDisplay.h"

CDisplay::CDisplay(Clcd* lcd) {
	//lcd->startup();
	//lcd->backlightOn();
	lcd->clear();
	lcd->gotoXY(0, 0);
}

bool CDisplay::clear(void) {
	lcd->clear();
	return true;
}

bool CDisplay::performSet(void) {
	lcd->clear();
	return true;
}

bool CDisplay::setLine(u08 line, const char* Str) {
	u08 len;
	len = strlen(Str);
	len = (len > LCD_MAX_COL) ? LCD_MAX_COL : len;
	lcd->gotoXY(0, line);
	lcd->puts((u08*) Str, len);
	return true;
}

bool CDisplay::setString(u08 StrLen, u08 xPos, u08 yPos, const char* Str) {
	StrLen = (StrLen > LCD_MAX_COL) ? LCD_MAX_COL : StrLen;
	lcd->gotoXY(xPos, yPos);
	lcd->puts((u08*) Str, StrLen);
	return true;
}

/*
 bool CDisplay::setChar(char* chr) {
 lcd->printData(&chr,1);
 return true;
 }
 */
/*
bool CDisplay::setString_P(u08 xPos, u08 yPos, prog_char *buf) {
	lcd->gotoXY(xPos, yPos);
	lcd->printData_P(buf, strlen_P(buf));
	return true;
}
*/

bool CDisplay::available(void) {
	return true;
}

