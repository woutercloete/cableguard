#ifndef CRC_H
#define CRC_H

#include "avrlibtypes.h"

#define CRC8INIT	0x00
#define CRC8POLY	0x18

u08 crc8(u08 *data_in, u16 number_of_bytes_to_read);
u16 crc16(u08 *pBuf, u16 len, u16 initialCRC);
u16 crctag(u08 *pBuf, u16 len, u16 initialCRC);

#endif
