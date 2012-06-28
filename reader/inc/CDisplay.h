#ifndef CDISPLAY_H_
#define CDISPLAY_H_

#include <avr/pgmspace.h>
#include "avrlibtypes.h"
#include "Clcd.h"

class CDisplay {
    Clcd* lcd;
  public:
    CDisplay(Clcd* lcd);
    bool available(void);
    bool clear(void);
    bool setLine(u08 line, const char* Str);
    bool setString(u08 StrLen, u08 xPos, u08 yPos, const char* Str);
    bool setString_P(u08 xPos, u08 yPos, prog_char *buf);
    bool performSet(void);

};

#endif /* CDISPLAY_H_ */
