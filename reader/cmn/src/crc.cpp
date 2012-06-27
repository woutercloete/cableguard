#include "crc.h"
//---------------------------------------------------------
u08 crc8(u08 *data_in, u16 number_of_bytes_to_read) {
  u08 crc;
  u16 loop_count;
  u08 bit_counter;
  u08 data;
  u08 feedback_bit;

  crc = CRC8INIT;

  for (loop_count = 0; loop_count != number_of_bytes_to_read; loop_count++) {
    data = data_in[loop_count];

    bit_counter = 8;
    do {
      feedback_bit = (crc ^ data) & 0x01;
      if (feedback_bit == 0x01) {
        crc = crc ^ CRC8POLY;
      }
      crc = (crc >> 1) & 0x7F;
      if (feedback_bit == 0x01) {
        crc = crc | 0x80;
      }

      data = data >> 1;

      bit_counter--;

    } while (bit_counter > 0);
  }

  return crc;
}

/*-----------------------------------------------------------------------------
NAME:           crc16
DESCRIPTION:    calculate crc16 checksum, polynomial x16 + x12 +x5 + 1
                An initial CRC value parameter is provided to allow calling this
                function several times and not loosing the CRC value
INPUTS:         pBuf:       pointer to byte buffer
                Len:        number of bytes in buffer
                InitialCrc: initial value of crc
OUTPUTS:        none
RETURN:         CRC16 value according above polynomial
-----------------------------------------------------------------------------*/
u16 crc16(u08 *pBuf, u16 len, u16 initialCRC) {
    u16 crc, i;

    crc = initialCRC;
    for (i = 0; i < len; i++ ) {
        crc = ((crc & 0xffff) >> 8) | ((crc & 0xffff) << 8);
        crc = (crc & 0xffff)  ^ pBuf[i];
        crc = (crc & 0xffff)  ^ ((crc & 0xffff) >> 4);
        crc = (crc & 0xffff)  ^ ((crc & 0xffff) << 12);
        crc = (crc & 0xffff)  ^ ((crc & 0xffff) << 5);
    }
    crc = (crc & 0xffff);
    return(crc);
}

/*-----------------------------------------------------------------------------
NAME:           crc16
DESCRIPTION:    calculate crc16 checksum, polynomial x16 + x12 +x5 + 1
                An initial CRC value parameter is provided to allow calling this
                function several times and not loosing the CRC value
INPUTS:         pBuf:       pointer to byte buffer
                Len:        number of bytes in buffer
                InitialCrc: initial value of crc
OUTPUTS:        none
RETURN:         CRC16 value according above polynomial
-----------------------------------------------------------------------------*/
u16 crctag(u08 *pBuf, u16 len, u16 initialCRC) {
    u16 crc, i;

    crc = initialCRC;
    for (i = 0; i < len; i++ ) {
        crc = (crc >> 8) | (crc << 8);
        crc = crc ^ pBuf[i];
        crc = crc ^ ((crc & 0xff) >> 4);
        crc = crc ^ (crc << 12);
        crc = crc ^ ((crc & 0xff) << 5);
    }
    return(crc);
}

