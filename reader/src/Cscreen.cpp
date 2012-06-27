/*
 * Cscreen.cpp
 *
 *  Created on: 06 Mar 2012
 *      Author: wouter
 */
#include <string.h>
#include "Cscreen.h"

CDisplay* Cscreen::displayInterface;
sCurrentScreen Cscreen::screen;
u08 Cscreen::displayTimer;
sDate* Cscreen::date;
Cstorage* Cscreen::store;
Csignal Cscreen::signal;
u08 Cscreen::idx[LCD_MAX_ROW];
u08 Cscreen::size[LCD_MAX_ROW];

void Cscreen::build(void) {
  size[0] = strlen((c08*) &screen.lines[0][0]);
  idx[0] = 0;
  size[1] = strlen((c08*) &screen.lines[1][0]);
  idx[1] = 0;
  //screen = screen;
  // Remove null terminators added by snprintf
  for (u08 cnt = 0; cnt < sizeof(screen); cnt++) {
    screen.line[cnt] = (screen.line[cnt] == 0) ? 32 : screen.line[cnt];
  }
}

void Cscreen::display(void) {
  c08 spaces[] = "                          ";
  // Display on LCD
  for (u08 cnt = 0; cnt < LCD_MAX_ROW; cnt++) {
    if (size[cnt] > LCD_MAX_COL) {
      displayInterface->setString(size[cnt] - idx[cnt], 0, cnt,
                                  (c08*) &screen.lines[cnt][idx[cnt]]);
      displayInterface->setString(idx[cnt], size[cnt] - idx[cnt], cnt, (c08*) spaces);
      idx[cnt] = (idx[cnt] + 1) % size[cnt];
    } else {
      displayInterface->setString(LCD_MAX_COL, 0, cnt, (c08*) &screen.lines[cnt][0]);
    }
  }
  signal.clear();
}

void Cscreen::ip2str(u08* str, sIP* ip) {
  snprintf((c08*) str, screenMaxCol, "%d.%d.%d.%d", ip->ip0, ip->ip1, ip->ip2, ip->ip3);
}

void Cscreen::mac2str(u08* str, sMacADR* mac) {
  snprintf((c08*) str, screenMaxCol, "%02X.%02X.%02X.%02X.%02X.%02X", mac->mac0,
           mac->mac1, mac->mac2, mac->mac3, mac->mac4, mac->mac5);
}
