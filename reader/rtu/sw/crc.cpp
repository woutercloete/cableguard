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

