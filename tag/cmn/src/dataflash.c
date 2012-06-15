#include <util/atomic.h>

#include "avrlibtypes.h"
#include "types.h"
#include "spi.h"
#include "dataflash.h"

//Dataflash opcodes
#define FlashPageRead				0x52	// Main memory page read
#define FlashToBuf1Transfer 		0x53	// Main memory page to buffer 1 transfer
#define Buf1Read					0x54	// Buffer 1 read
#define FlashToBuf2Transfer 		0x55	// Main memory page to buffer 2 transfer
#define Buf2Read					0x56	// Buffer 2 read
#define StatusReg					0x57	// Status register
#define AutoPageReWrBuf1			0x58	// Auto page rewrite through buffer 1
#define AutoPageReWrBuf2			0x59	// Auto page rewrite through buffer 2
#define FlashToBuf1Compare    		0x60	// Main memory page to buffer 1 compare
#define FlashToBuf2Compare	    	0x61	// Main memory page to buffer 2 compare
#define ContArrayRead				0x68	// Continuous Array Read (Note : Only A/B-parts supported)
#define FlashProgBuf1				0x82	// Main memory page program through buffer 1
#define Buf1ToFlashWE   			0x83	// Buffer 1 to main memory page program with built-in erase
#define Buf1Write					0x84	// Buffer 1 write
#define FlashProgBuf2				0x85	// Main memory page program through buffer 2
#define Buf2ToFlashWE   			0x86	// Buffer 2 to main memory page program with built-in erase
#define Buf2Write					0x87	// Buffer 2 write
#define Buf1ToFlash     			0x88	// Buffer 1 to main memory page program without built-in erase
#define Buf2ToFlash		         	0x89	// Buffer 2 to main memory page program without built-in erase
void DF_BufferToPage(unsigned char BufferNo, unsigned int PageAdr);
unsigned char df_read_status(void);

// Globals
static unsigned char PageBits;
static unsigned int PageSize;

void DF_Init(void) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    DF_CS_INIT(); //Data Flah CS Initialize
    DF_CS_HIGH();
    DF_SPI_INIT();
    SPI0_MODE3();
    df_read_status();
  }
}

unsigned char DF_BufferReadByte(unsigned char BufferNo, unsigned int IntPageAdr) {
  unsigned char data = 0;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    DF_CS_LOW();
    if (1 == BufferNo) {
      DF_SPI_RW(Buf1Read);
      //buffer 1 read op-code
      DF_SPI_RW(0x00);
      //don't cares
      DF_SPI_RW((unsigned char)(IntPageAdr>>8));
      //upper part of internal buffer address
      DF_SPI_RW((unsigned char)(IntPageAdr));
      //lower part of internal buffer address
      DF_SPI_RW(0x00);
      //don't cares
      data = DF_SPI_RW(0x00); //read byte
    }
    DF_CS_HIGH();
  }
  return data;
}

void DF_BufferWriteByte(unsigned char BufferNo, unsigned int IntPageAdr,
                        unsigned char Data) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    DF_CS_LOW();
    if (1 == BufferNo) //write byte to buffer 1
        {
      DF_SPI_RW(Buf1Write);
      //buffer 1 write op-code
      DF_SPI_RW(0x00);
      //don't cares
      DF_SPI_RW((unsigned char)(IntPageAdr>>8));
      //upper part of internal buffer address
      DF_SPI_RW((unsigned char)(IntPageAdr));
      //lower part of internal buffer address
      DF_SPI_RW(Data);
      //write data byte
    }
    DF_CS_HIGH();
  }
}

/*****************************************************************************
 *
 *	Function name : df_read_status
 *
 *	Returns :		One status byte. Consult Dataflash datasheet for further
 *					decoding info
 *
 *	Parameters :	None
 *
 *	Purpose :		Status info concerning the Dataflash is busy or not.
 *					Status info concerning compare between buffer and flash page
 *					Status info concerning size of actual device
 *
 ******************************************************************************/
unsigned char df_read_status(void) {
  unsigned char result, index_copy;
  //Look-up table for these sizes ->  512k, 1M,  2M,  4M,  8M, 16M, 32M, 64M
  unsigned int DF_pagesize[] = { 264, 264, 264, 264, 264, 528, 528, 1056 };
  //Look-up table for these sizes ->  512k, 1M, 2M, 4M, 8M, 16M, 32M, 64M
  unsigned char DF_pagebits[] = { 9, 9, 9, 9, 9, 10, 10, 11 };

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    //make sure to toggle CS signal in order
    DF_CS_LOW(); //to reset dataflash command decoder
    result = DF_SPI_RW(StatusReg); //send status register read op-code
    result = DF_SPI_RW(0x00); //dummy write to get result
    index_copy = ((result & 0x38) >> 3); //get the size info from status register
    PageBits = DF_pagebits[index_copy]; //get number of internal page address bits from look-up table
    PageSize = DF_pagesize[index_copy]; //get the size of the page (in bytes)
    DF_CS_HIGH();
  }
  return result;
}

/*****************************************************************************
 *
 *	Function name : DF_PageToBuffer
 *
 *	Returns :		None
 *
 *	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
 *					PageAdr		->	Address of page to be transferred to buffer
 *
 *	Purpose :		Transfers a page from flash to dataflash SRAM buffer
 *
 ******************************************************************************/
void DF_PageToBuffer(unsigned char BufferNo, unsigned int PageAdr) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    DF_CS_LOW(); //to reset dataflash command decoder
    if (1 == BufferNo) {
      DF_SPI_RW(FlashToBuf1Transfer);
      //transfer to buffer 1 op-code
      DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));
      //upper part of page address
      DF_SPI_RW((unsigned char)(PageAdr << (PageBits - 8)));
      //lower part of page address
      DF_SPI_RW(0x00);
      //don't cares
    }
    DF_CS_HIGH(); //initiate the transfer
    DF_CS_LOW();
    while (!(df_read_status() & 0x80))
      ; //monitor the status register, wait until busy-flag is high
    DF_CS_HIGH();
  }
}


void DF_read(u08* dst, u16 srcPageNum) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // Transfer from FLASH page to FLASH SRAM
    DF_CS_LOW();
    DF_SPI_RW(FlashToBuf1Transfer);
    DF_SPI_RW((u08)(srcPageNum >> 7));
    DF_SPI_RW((u08)(srcPageNum << 1));
    DF_SPI_RW(0x00);
    DF_CS_HIGH();
    DF_CS_LOW();
    while (!(df_read_status() & 0x80)) {
    };
    DF_CS_HIGH();

    // Transfer from SRAM to internal dst
    DF_CS_LOW();
    DF_SPI_RW(Buf1Read);
    DF_SPI_RW(0x00);
    DF_SPI_RW(0x00);
    DF_SPI_RW(0x00);
    DF_SPI_RW(0x00);
    for (u16 i = 0; i < PageSize; i++) {
      dst[i] = DF_SPI_RW(0x00);
    }
    DF_CS_HIGH();
  }

}

void DF_write(u16 dstPageNum, u08* src) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // Transfer from src to the FLASH SRAM
    DF_CS_LOW();
    DF_SPI_RW(Buf1Write);
    DF_SPI_RW(0x00);
    DF_SPI_RW(0x00);
    DF_SPI_RW(0x00);
    for (u16 i = 0; i < PageSize; i++) {
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
    while (!(df_read_status() & 0x80)) {
    };
    DF_CS_HIGH();
  }
}

/*****************************************************************************
 *
 *	Function name : DF_BufferReadStr
 *
 *	Returns :		None
 *
 *	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
 *					IntPageAdr	->	Internal page address
 *					No_of_bytes	->	Number of bytes to be read
 *					*BufferPtr	->	address of buffer to be used for read bytes
 *
 *	Purpose :		Reads one or more bytes from one of the dataflash
 *					internal SRAM buffers, and puts read bytes into
 *					buffer pointed to by *BufferPtr
 *
 ******************************************************************************/
void DF_BufferReadStr(unsigned char BufferNo, unsigned int IntPageAdr,
                      unsigned int No_of_bytes, unsigned char *BufferPtr) {
  unsigned int i;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    DF_CS_LOW(); //to reset dataflash command decoder
    if (1 == BufferNo) {
      DF_SPI_RW(Buf1Read);
      //buffer 1 read op-code
      DF_SPI_RW(0x00);
      //don't cares
      DF_SPI_RW((unsigned char)(IntPageAdr>>8));
      //upper part of internal buffer address
      DF_SPI_RW((unsigned char)(IntPageAdr));
      //lower part of internal buffer address
      DF_SPI_RW(0x00);
      //don't cares
      for (i = 0; i < No_of_bytes; i++) {
        *(BufferPtr) = DF_SPI_RW(0x00); //read byte and put it in AVR buffer pointed to by *BufferPtr
        BufferPtr++; //point to next element in AVR buffer
      }
    }
    DF_CS_HIGH();
  }
}
//NB : Sjekk at (IntAdr + No_of_bytes) < buffersize, hvis ikke blir det bare ball..

/*****************************************************************************
 *
 *	Function name : DF_BufferWriteEnable
 *
 *	Returns :		None
 *
 *	Parameters :	IntPageAdr	->	Internal page address to start writing from
 *					BufferAdr	->	Decides usage of either buffer 1 or 2
 *
 *	Purpose :		Enables continous write functionality to one of the dataflash buffers
 *					buffers. NOTE : User must ensure that CS goes high to terminate
 *					this mode before accessing other dataflash functionalities
 *
 ******************************************************************************/
void DF_BufferWriteEnable(unsigned char BufferNo, unsigned int IntPageAdr) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    DF_CS_LOW(); //to reset dataflash command decoder

    if (1 == BufferNo) {
      DF_SPI_RW(Buf1Write);
      //buffer 1 write op-code
      DF_SPI_RW(0x00);
      //don't cares
      DF_SPI_RW((unsigned char)(IntPageAdr>>8));
      //upper part of internal buffer address
      DF_SPI_RW((unsigned char)(IntPageAdr));
      //lower part of internal buffer address
    }
    DF_CS_HIGH();
  }
}

/*****************************************************************************
 *
 *	Function name : DF_BufferWriteStr
 *
 *	Returns :		None
 *
 *	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
 *					IntPageAdr	->	Internal page address
 *					No_of_bytes	->	Number of bytes to be written
 *					*BufferPtr	->	address of buffer to be used for copy of bytes
 *									from AVR buffer to dataflash buffer 1 (or 2)
 *
 *	Purpose :		Copies one or more bytes to one of the dataflash
 *					internal SRAM buffers from AVR SRAM buffer
 *					pointed to by *BufferPtr
 *
 ******************************************************************************/
void DF_BufferWriteStr(unsigned char BufferNo, unsigned int IntPageAdr,
                       unsigned int No_of_bytes, unsigned char *BufferPtr) {
  unsigned int i;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    DF_CS_LOW(); //to reset dataflash command decoder
    if (1 == BufferNo) {
      DF_SPI_RW(Buf1Write);
      //buffer 1 write op-code
      DF_SPI_RW(0x00);
      //don't cares
      DF_SPI_RW((unsigned char)(IntPageAdr>>8));
      //upper part of internal buffer address
      DF_SPI_RW((unsigned char)(IntPageAdr));
      //lower part of internal buffer address
      for (i = 0; i < No_of_bytes; i++) {
        DF_SPI_RW(*(BufferPtr));
        //write byte pointed at by *BufferPtr to dataflash buffer 1 location
        BufferPtr++; //point to next element in AVR buffer
      }
    }
    DF_CS_HIGH();
  }
}
//NB : Monitorer busy-flag i status-reg.
//NB : Sjekk at (IntAdr + No_of_bytes) < buffersize, hvis ikke blir det bare ball..

/*****************************************************************************
 *
 *	Function name : Cont_Flash_Read_Enable
 *
 *	Returns :		None
 *
 *	Parameters :	PageAdr		->	Address of flash page where cont.read starts from
 *					IntPageAdr	->	Internal page address where cont.read starts from
 *
 *	Purpose :		Initiates a continuous read from a location in the DataFlash
 *
 ******************************************************************************/
void Cont_Flash_Read_Enable(unsigned int PageAdr, unsigned int IntPageAdr) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    DF_CS_LOW(); //to reset dataflash command decoder
    DF_SPI_RW(ContArrayRead);
    //Continuous Array Read op-code
    DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));
    //upper part of page address
    DF_SPI_RW((unsigned char)((PageAdr << (PageBits - 8))+ (IntPageAdr>>8)));
    //lower part of page address and MSB of int.page adr.
    DF_SPI_RW((unsigned char)(IntPageAdr));
    //LSB byte of internal page address
    DF_SPI_RW(0x00);
    //perform 4 dummy writes
    DF_SPI_RW(0x00);
    //in order to intiate DataFlash
    DF_SPI_RW(0x00);
    //address pointers
    DF_SPI_RW(0x00);
    DF_CS_HIGH();
  }
}

