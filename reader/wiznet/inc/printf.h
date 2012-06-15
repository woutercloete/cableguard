#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <stdio.h>
#include <avr/pgmspace.h>

#include "global.h"
  #define PRINTF(format, ...) printf_P(PSTR(format), ## __VA_ARGS__)
#endif
