#ifndef SPIEEPROM_H
#define SPIEEPROM_H

#include "avrlibtypes.h"
#include "avrlibdefs.h"
#include "CSpi.h"

// defines and constants
// commands
#define SPIEEPROM_CMD_READ		0x03	///< Read byte(s)
#define SPIEEPROM_CMD_WRITE		0x02	///< Write byte(s)
#define SPIEEPROM_CMD_WREN		0x06	///< Write Enable
#define SPIEEPROM_CMD_WRDI		0x04	///< Write Disable
#define SPIEEPROM_CMD_RDSR		0x05	///< Read Status Register
#define SPIEEPROM_CMD_WRSR		0x01	///< Write Status Register

// status register bit defines
#define SPIEEPROM_STATUS_WIP	0x01	///< Write in progress
#define SPIEEPROM_STATUS_WEL	0x01	///< Write enable
#define SPIEEPROM_STATUS_BP0	0x01	///< Block Proection 0
#define SPIEEPROM_STATUS_BP1	0x01	///< Block Proection 1
#define SPIEEPROM_STATUS_WPEN	0x01	///< Write Protect Enable

// functions

#define CS_EN()    cbi(PORTB,0);
#define CS_DIS()   sbi(PORTB,0);  _NOP();_NOP();_NOP();_NOP();\
                                  _NOP();_NOP();_NOP();_NOP();_NOP();_NOP();


class CFram {
  private:
    CSpi *spi;
    void wr_enable(void);
    void wr_disable(void);
    u08 readstatus(void);
  public:
    CFram(CSpi *_spi);

    u08 readbyte(u32 memAddr);
    void readblock(u32 memAddr, u08 *data, u32 len);

    void writebyte(u32 memAddr, u08 data);
    void writeblock(u32 memAddr, u08 *data, u32 len);
};

#endif
