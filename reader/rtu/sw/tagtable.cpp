/****************************************************************************************/
// This class contains all the visible tags and an event tags.
//
//   Created : 15 March 2012
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
/****************************************************************************************/
#include <string.h>
#include <stdio.h>
#include <avr/eeprom.h>
/****************************************************************************************/
#include "tagtable.h"
/****************************************************************************************/
Ctag tagList[MAX_NUM_TAGS];
/****************************************************************************************/
