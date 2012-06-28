#include "spi.h"

void SPI0_Init(void)
{
	SPI0_SS_WR();
	SPI0_SS_HIGH();

	SPI0_SCLK_WR();
	SPI0_SCLK_HIGH();

	SPI0_MOSI_WR();
	SPI0_MISO_RD();

	SPI0_ENABLE();
	SPI0_MASTER_MODE();
	
	SPI0_MODE0();

	SPI0_CLK_DOULBE();
	SPI0_CLK_DIV4();
}
	
//----------------------------------------------------------------
unsigned char SPI0_WriteReadByte(unsigned char Data)
{
	SPI0_TxData(Data);	

	SPI0_WaitForSend();
 
	return SPI0_RxData();
}

void SPI0_WriteByte(unsigned char Data)
{
	SPI0_TxData(Data);	

	SPI0_WaitForSend();	
}
//----------------------------------------------------------------

