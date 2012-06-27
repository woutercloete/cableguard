#include <stdio.h>
#include <string.h>
#include "types.h"
#include "serial.h"
#include "evb.h"
#include "dataflash.h"
#include "romfile.h"


unsigned int m_WriteAddress = 0;
unsigned int m_WritePage = 0;

void InitRomFile(void)
{
	DF_Init();
	InitDataFlash();
}

void InitDataFlash(void)
{
	m_WriteAddress = 0;
	m_WritePage = 0;
}

void write_to_flash(unsigned char flash_data)
{
	DF_BufferWriteByte(1, m_WriteAddress, flash_data);
        
    m_WriteAddress++;

	// if buffer full write buffer into memory page
    if (m_WriteAddress>(PAGE_PER_BYTE-1))               
    {
        m_WriteAddress = 0;
		
        if (m_WritePage < 2048)            // if memory is not full   
        { 
			DF_BufferToPage(1, m_WritePage);
            m_WritePage++;
        }
        else
        {

        }
    }
}


void write_end_page(void)
{
	DF_BufferToPage(1, m_WritePage);
}

void read_from_flashbuf(unsigned long Address, unsigned char* Buffer, unsigned int Size)
{
    unsigned int page = 0;
    unsigned int address = 0;	
	
	unsigned int page_no = 0;
	unsigned int index = 0;
	unsigned int read_size = 0;
	unsigned int i;

	page_no = (unsigned int)Size/PAGE_PER_BYTE;
	
	page = (unsigned int)(Address/(PAGE_PER_BYTE));
	address	= (unsigned int)(Address%(PAGE_PER_BYTE));

	if(Address == 0){page = 0;address=0;}
	DF_PageToBuffer(1, page);	
	
	read_size = PAGE_PER_BYTE-address;
	if(read_size >= Size)
	{
		read_size = Size;
	}
	else read_size = PAGE_PER_BYTE-address;
	
	DF_BufferReadStr(1, address, read_size, &Buffer[0]);
	index = read_size;
	page++;
	
	for(i=0;i<page_no;i++)
	{
		DF_PageToBuffer(1, page++);
		DF_BufferReadStr(1, 0, PAGE_PER_BYTE, &Buffer[index]);
		index += PAGE_PER_BYTE;
		read_size += PAGE_PER_BYTE;
	}
	
	if(Size<=read_size)return;
	read_size = Size - read_size; 
	
	if(read_size>0)
	{
		DF_PageToBuffer(1, page);
		DF_BufferReadStr(1, 0, read_size, &Buffer[index]);	
	}
}


unsigned char search_file_rom(unsigned char *FileName, unsigned long *Address, unsigned long *Size)
{
	unsigned char fileidx = 1;

	ROM_DIR file;
	unsigned int addr = 0;

	while(1)
	{
		read_from_flashbuf((unsigned long)addr, (unsigned char*)&file.b, sizeof(file));

        if(file.b[0]==0xff && file.b[1]==0xff)
		{
			break;
		}
		
		//search file
		if(!strcmp((char*)FileName, file.f.name))
		{
			*Size = file.f.len;
			*Address = file.f.start;

			return fileidx;
		}
		addr += FILE_HEAD_SIZE;
		fileidx++;
		
	}
	
	fileidx = 0;
	
	return fileidx;
}
