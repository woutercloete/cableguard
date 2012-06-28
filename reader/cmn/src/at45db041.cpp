/****************************************************************************************/
#include <util/atomic.h>
#include "at45db041.h"
/****************************************************************************************/
Cat45db041::Cat45db041(void) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		DF_CS_INIT();
		DF_CS_HIGH();
		DF_SPI_INIT();
		status();
	}
}
/****************************************************************************************/
u08 Cat45db041::status(void) {
	u08 result;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		SPI0_MODE3();
		DF_CS_LOW();
		result = DF_SPI_RW(StatusReg);
		result = DF_SPI_RW(0x00);
		DF_CS_HIGH();
	}
	return result;
}
/****************************************************************************************/
void Cat45db041::read(u08* dst, u16 srcPageNum) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		SPI0_MODE3();
		// Transfer from FLASH page to FLASH SRAM
		DF_CS_LOW();
		DF_SPI_RW(FlashToBuf1Transfer);
		DF_SPI_RW((u08)(srcPageNum >> 7));
		DF_SPI_RW((u08)(srcPageNum << 1));
		DF_SPI_RW(0x00);
		DF_CS_HIGH();
		DF_CS_LOW();
		while (!(status() & 0x80)) {
		};
		DF_CS_HIGH();

		// Transfer from SRAM to internal dst
		DF_CS_LOW();
		DF_SPI_RW(Buf1Read);
		DF_SPI_RW(0x00);
		DF_SPI_RW(0x00);
		DF_SPI_RW(0x00);
		DF_SPI_RW(0x00);
		for (u16 i = 0; i < pageSize; i++) {
			dst[i] = DF_SPI_RW(0x00);
		}
		DF_CS_HIGH();
	}
}
/****************************************************************************************/
void Cat45db041::write(u16 dstPageNum, u08* src) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		// Transfer from src to the FLASH SRAM
		SPI0_MODE3();
		DF_CS_LOW();
		DF_SPI_RW(Buf1Write);
		DF_SPI_RW(0x00);
		DF_SPI_RW(0x00);
		DF_SPI_RW(0x00);
		for (u16 i = 0; i < pageSize; i++) {
			DF_SPI_RW(src[i]);
		}
		DF_CS_HIGH();

		// Transfer from FLASH SRAM to FLASH dst page
		DF_CS_LOW();
		DF_SPI_RW(Buf1ToFlashWE);
		DF_SPI_RW((u08)(dstPageNum >> 7));
		DF_SPI_RW((u08)(dstPageNum << 1));
		DF_SPI_RW(0x00);
		DF_CS_HIGH();
		DF_CS_LOW();
		while (!(status() & 0x80)) {
		};
		DF_CS_HIGH();
	}
}
