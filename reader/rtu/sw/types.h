/*! \file avrlibtypes.h \brief AVRlib global types and typedefines. */
//*****************************************************************************
//
// File Name	: 'avrlibtypes.h'
// Title		: AVRlib global types and typedefines include file
// Author		: Pascal Stang
// Created		: 7/12/2001
// Revised		: 9/30/2002
// Version		: 1.0
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
//	Description : Type-defines required and used by AVRlib.  Most types are also
//						generally useful.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************


#ifndef AVRLIBTYPES_H
#define AVRLIBTYPES_H

#ifndef WIN32
	// true/false defines
	#define FALSE	0
	#define TRUE	1
#endif

#define true (1==1)
#define false (0==1)


#define SUCCESS  0
#define ERROR    0xFF
#define TIMEOUT  0xFE

#define PRIVATE     static
#define PUBLIC

#ifdef __cplusplus
#define EXTERN      extern "C"
#else
#define EXTERN      extern
#endif


// data type definitions macros
#ifndef __cplusplus
typedef unsigned char  bool;
#endif
typedef unsigned char  u08;
typedef   signed char  s08;
typedef          char  c08;
typedef unsigned short u16;
typedef   signed short s16;
typedef unsigned long  u32;
typedef   signed long  s32;
typedef unsigned long long u64;
typedef   signed long long s64;
typedef              float f32;

#define pc08 EEMEM c08

typedef union
{
  u32 value;
  u08 array[4];
}LONG_DATA;


typedef void (*voidFuncPtr)(void);

typedef void (*u08FuncPtr)(u08);

//typedef u08 (* const u08FuncPtr_Retu08)(*u08);


/* use inttypes.h instead
// C99 standard integer type definitions
typedef unsigned char	uint8_t;
typedef   signed char	int8_t;
typedef unsigned short	uint16_t;
typedef   signed short	int16_t;
typedef unsigned long	uint32_t;
typedef   signed long	int32_t;
typedef unsigned long	uint64_t;
typedef   signed long	int64_t;
*/
// maximum value that can be held
// by unsigned data types (8,16,32bits)
#define MAX_U08	255
#define MAX_U16	65535
#define MAX_U32	4294967295

// maximum values that can be held
// by signed data types (8,16,32bits)
#define MIN_S08	-128
#define MAX_S08	127
#define MIN_S16	-32768
#define MAX_S16	32767
#define MIN_S32	-2147483648
#define MAX_S32	2147483647

// Code compatibility to new AVR-libc
// outb(), inb(), inw(), outw(), BV(), sbi(), cbi(), sei(), cli()
#ifndef outb
#define outb(addr, data)  addr = (data)
#endif
#ifndef inb
#define inb(addr)     (addr)
#endif
#ifndef outw
#define outw(addr, data)  addr = (data)
#endif
#ifndef inw
#define inw(addr)     (addr)
#endif
#ifndef BV
#define BV(bit)     (1<<(bit))
#endif
#ifndef cbi
#define clearbit(reg,bit)  reg &= ~(BV(bit))
#endif
#ifndef sbi
#define setbit(reg,bit)  reg |= (BV(bit))
#endif
#ifndef cli
#define cli()     __asm__ __volatile__ ("cli" ::)
#endif
#ifndef sei
#define sei()     __asm__ __volatile__ ("sei" ::)
#endif

// support for individual port pin naming in the mega128
// see port128.h for details
#ifdef __AVR_ATmega128__
// not currently necessary due to inclusion
// of these defines in newest AVR-GCC
// do a quick test to see if include is needed
#ifndef PD0
#include "port128.h"
#endif
#endif

// use this for packed structures
// (this is seldom necessary on an 8-bit architecture like AVR,
//  but can assist in code portability to AVR)
#define GNUC_PACKED __attribute__((packed))

// port address helpers
#define DDR(x) ((x)-1)    // address of data direction register of port x
#define PIN(x) ((x)-2)    // address of input register of port x
// MIN/MAX/ABS macros
#define MIN(a,b)      ((a<b)?(a):(b))
#define MAX(a,b)      ((a>b)?(a):(b))
#define ABS(x)        ((x>0)?(x):(-x))

// constants
#define PI    3.14159265359

/// Macro to extract the high 8 bits of a 16-bit value (Most Significant Byte)
#define U16_HI(u16Data) ((u08)((u16Data>>8)&0xff))

/// Macro to extract the low 8 bits of a 16-bit value (Least Significant Byte)
#define U16_LO(u16Data) ((u08)(u16Data&0xff))


#endif
