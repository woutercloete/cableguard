#ifndef SPI_H
#define SPI_H

#include "avrlibtypes.h"
#include "avrlibdefs.h"

class CSpi {
  private:
  public:
    volatile u08 TransferComplete;
    CSpi(void);
    void sendbyte(u08 data); //sends a single byte over the SPI port
    u08 transferbyte(u08 data);//sends a single byte over the SPI port & returns the byte that was received during transmission.
    void transferBlock(u08* data, u08* buffer, u08 len);
    void sendBlock(u08* data, u08 len);
};

#endif
