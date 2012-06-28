#ifndef _SPI_H
#define _SPI_H

#include "avrlibdefs.h"
#include "avrlibtypes.h"


#define SPI_SS   2
#define SPI_MOSI 3
#define SPI_MISO 4
#define SPI_SCK  5

class SPI
{
  public:
    void init();
    u08 send(u08 value);
};
#endif
