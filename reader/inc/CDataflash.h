/******** Example Code : Accessing Atmel AT45Dxxx dataflash on STK500 *******

Device      : 	AT90S8515

File name   : 	dataflash.h

Description : 	Defines and prototypes for AT45Dxxx
                 
Last change:    16 Aug 2001   AS
 
****************************************************************************/

#ifndef __DATAFLASH_INCLUDED
#define __DATAFLASH_INCLUDED
#endif

#include <stdio.h>

#define DATAFLASH_CS_PORT   PORTB
#define DATAFLASH_CS_PIN    0


#define SetBit(x,y)		(x |= (y))
#define ClrBit(x,y)		(x &=~(y))
#define ChkBit(x,y)		(x  & (y))


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

// CJB 25/4/2008: AT26 commands
#define ChipErase					0x60	// AT26 family chip erase
#define BlockErase4					0x20	// AT26 family 4KB block erase
#define WriteEnable					0x06	// AT26 family write enable
#define WriteDisable				0x04	// AT26 family write disable
#define ReadArray					0x03	// AT26 family read array
#define SeqProgram					0xad	// AT26 sequential program mode
#define ReadStatusReg				0x05	// AT26 read status register
#define ReadManufID					0x9f	// AT26 read manufacturer ID
#define WriteStatusReg				0x01	// AT26 write status register

//Dataflash macro definitions
#define DF_CS_active	cbi(DATAFLASH_CS_PORT,DATAFLASH_CS_PIN)
#define DF_CS_inactive	sbi(DATAFLASH_CS_PORT,DATAFLASH_CS_PIN)

#define FLASH_PAGE_SIZE	264
//#define FLASH_BUFFER	2048
#define FLASH_BUFFER	1024
//#define FLASH_BUFFER	256

//Function definitions
void DF_SPI_init (void);
unsigned char DF_SPI_RW (unsigned char output);
unsigned char Read_DF_status (void);
void Page_To_Buffer (unsigned int PageAdr, unsigned char BufferNo);
unsigned char Buffer_Read_Byte (unsigned char BufferNo, unsigned int IntPageAdr);
void Buffer_Read_Str (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr);
void Buffer_Write_Enable (unsigned char BufferNo, unsigned int IntPageAdr);
void Buffer_Write_Byte (unsigned char BufferNo, unsigned int IntPageAdr, unsigned char Data);
void Buffer_Write_Str (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr);
void Buffer_To_Page (unsigned char BufferNo, unsigned int PageAdr);
void Cont_Flash_Read_Enable (unsigned int PageAdr, unsigned int IntPageAdr);

// Functions for the AT26 family
unsigned char Read_DF26_status (void);
void Chip_Erase (void);
void Block_Erase_4 (u32 IntPageAdr);
void Sequential_Program (u32 IntPageAdr, u32 No_of_bytes, unsigned char *BufferPtr);
void Read_Array (u32 IntPageAdr, u32 No_of_bytes, unsigned char *BufferPtr);
void Read_ID (unsigned char *BufferPtr);
unsigned char Wait_DF26_ready (void);

void setInactive(void);
void setActive(void);
