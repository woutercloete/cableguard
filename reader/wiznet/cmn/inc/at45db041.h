#ifndef AT45DB041_H_
#define AT45DB041_H_
/****************************************************************************************/
#include <avr/io.h>
/****************************************************************************************/
#include "avrlibtypes.h"
#include "types.h"
/****************************************************************************************/
extern "C" {
#include "spi.h"
}
/****************************************************************************************/
#define DF_SPI_INIT()				SPI0_Init()
#define DF_CS_BIT					  BIT4
#define DF_CS_INIT()				Sbi(DDRG, DF_CS_BIT)
#define DF_CS_LOW()					Cbi(PORTG, DF_CS_BIT)
#define DF_CS_HIGH()				Sbi(PORTG, DF_CS_BIT)
#define DF_SPI_RW(Data)			SPI0_WriteReadByte(Data)
/****************************************************************************************/
namespace AT45DB041 {
  const u16 pageSize = 256;
}
/****************************************************************************************/
using namespace AT45DB041;
/****************************************************************************************/
typedef enum {
  FlashPageRead = 0x52,
    FlashToBuf1Transfer = 0x53,
    Buf1Read = 0x54,
    FlashToBuf2Transfer = 0x55,
    Buf2Read = 0x56,
    StatusReg = 0x57,
    AutoPageReWrBuf1 = 0x58,
    AutoPageReWrBuf2 = 0x59,
    FlashToBuf1Compare = 0x60,
    FlashToBuf2Compare = 0x61,
    ContArrayRead = 0x68,
    FlashProgBuf1 = 0x82,
    Buf1ToFlashWE = 0x83,
    Buf1Write = 0x84,
    FlashProgBuf2 = 0x85,
    Buf2ToFlashWE = 0x86,
    Buf2Write = 0x87,
    Buf1ToFlash = 0x88,
    Buf2ToFlash = 0x89
} eOpcode;
/****************************************************************************************/
class Cat45db041 {
  public:
    Cat45db041();
    void read(u08* dst, u16 srcPageNum);
    void write(u16 dstPageNum, u08* src);
    u08 status();
};
/****************************************************************************************/
#endif
