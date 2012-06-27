/*! \file global.h \brief AVRlib project global include. */
//*****************************************************************************
//
// File Name	: 'global.h'
// Title		: AVRlib project global include
// Author		: Pascal Stang - Copyright (C) 2001-2002
// Created		: 7/12/2001
// Revised		: 9/30/2002
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
//	Description : This include file is designed to contain items useful to all
//					code files and projects.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef GLOBAL_H
#define GLOBAL_H

#ifndef WIN32
#define CYCLES_PER_US ((F_CPU+500000)/1000000) 	// cpu cycles per microsecond
#define US 1000
#define MS 1000000

/// Macro to set a port bit (1)
#define BIT_SET_HI(Port,Bit)             {Port |=(1<<Bit);}

/// Macro to clear a port bit (0)
#define BIT_SET_LO(Port,Bit)             {Port &= ~(1<<Bit);}

/// Macro to toggle a port bit
#define BIT_TOGGLE(Port,Bit)             {if(Port&(1<<Bit)) {Port &= ~(1<<Bit);} else {Port |=(1<<Bit);}}

/// Macro to test if a port bit is set (1?)
#define BIT_IS_HI(Port,Bit)              ((Port&(1<<Bit)) != 0)

/// Macro to test if a port bit is cleared (0?)
#define BIT_IS_LO(Port,Bit)              ((Port&(1<<Bit)) == 0)

/// Macro to wait until a port bit is set
#define LOOP_UNTIL_BIT_IS_HI(Port,Bit)   while(BIT_IS_LO(Port,Bit)) {;}

/// Macro to wait until a port bit is cleared
#define LOOP_UNTIL_BIT_IS_LO(Port,Bit)   while(BIT_IS_HI(Port,Bit)) {;}
//@}

///  \name Byte macros
//@{
/// Macro to extract the high 8 bits of a 16-bit value (Most Significant Byte)
#define U16_HI(u16Data) ((u08)((u16Data>>8)&0xff))

/// Macro to extract the low 8 bits of a 16-bit value (Least Significant Byte)
#define U16_LO(u16Data) ((u08)(u16Data&0xff))
//@}

/// \name Utility macros
//@{
/// Macro to calculate division with rounding to nearest integer value
#define DIV(Dividend,Divisor) (((Dividend+((Divisor)>>1))/(Divisor)))

/// Macro to check if a value is within bounds (Min <= Value <= Max ?)
#define BOUND(Value,Min,Max)    (((Value)>=(Min))&&((Value)<=(Max)))

/// Macro to calculate the length of an array
#define ARRAY_LENGTH(Array)      (sizeof(Array)/sizeof((Array)[0]))
//@}
#define PRINTF(format, ...) printf_P(PSTR(format), ## __VA_ARGS__)

#define DEF8x   defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__)
#define DEF8    defined(__AVR_ATmega8__)
#define NDEF8x  !(DEF8x)
#define DEF164  defined(__AVR_ATmega164P__ || defined(__AVR_ATmega644__))
#define NDEF164 !(DEF164)
#endif
#endif
