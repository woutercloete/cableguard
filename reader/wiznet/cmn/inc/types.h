/*
*
@file		type.h
*
*/

#ifndef _TYPE_H_
#define _TYPE_H_


/***************************************************
 * attribute for mcu ( types, ... ) 
 ***************************************************/
//#include "mcu_define.h"
#define __MCU_AVR__	1
#define __MCU_TYPE__	__MCU_AVR__

//---- Refer "Rom File Maker Manual Vx.x.pdf"
#include <avr/pgmspace.h>

#define	_ENDIAN_LITTLE_	0	/**<  This must be defined if system is little-endian alignment */
#define	_ENDIAN_BIG_		1
#define 	SYSTEM_ENDIAN		_ENDIAN_LITTLE_

#define	MAX_SOCK_NUM		8	/**< Maxmium number of socket  */
#define	CLK_CPU			8000000		/**< 8Mhz(for serial) */

/* ## __DEF_IINCHIP_xxx__ : define option for iinchip driver *****************/
//#define __DEF_IINCHIP_DBG__ /* involve debug code in driver (socket.c) */
//#define __DEF_IINCHIP_INT__ /**< involve interrupt service routine (socket.c) */
//#define __DEF_IINCHIP_PPP__ /* involve pppoe routine (socket.c) */
                            /* If it is defined, the source files(md5.h,md5.c) must be included in your project.
                               Otherwize, the source files must be removed in your project. */

#define __DEF_IINCHIP_DIRECT_MODE__ 1
#define __DEF_IINCHIP_INDIRECT_MODE__ 2
//#define __DEF_IINCHIP_SPI_MODE__ 3

#define __DEF_IINCHIP_BUS__ __DEF_IINCHIP_DIRECT_MODE__
//#define __DEF_IINCHIP_BUS__ __DEF_IINCHIP_INDIRECT_MODE__
//#define __DEF_IINCHIP_BUS__ __DEF_IINCHIP_SPI_MODE__ /*Enable SPI_mode*/


#define __DEF_IINCHIP_ADDRESS_MODE__           __DEF_IINCHIP_DIRECT_MODE__
//#define __DEF_IINCHIP_ADDRESS_MODE__         __DEF_IINCHIP_INDIRECT_MODE__

/**
@brief	 __DEF_IINCHIP_MAP_xxx__ : define memory map for iinchip 
*/
#define __DEF_IINCHIP_MAP_BASE__ 0x8000
#if (__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_DIRECT_MODE__)
 #define COMMON_BASE __DEF_IINCHIP_MAP_BASE__
 #define COMMON_REG_BASE __DEF_IINCHIP_MAP_BASE__
 #define SOCKET_REG_BASE   __DEF_IINCHIP_MAP_BASE__ + 0x0200 /**< The base address of SOCKET_REG */     
  
#else
 #define COMMON_BASE 0x0000
 #define COMMON_REG_BASE 	0
 #define SOCKET_REG_BASE   0x0200 /**< The base address of SOCKET_REG */       
#endif
#define __DEF_IINCHIP_MAP_TXBUF__ (COMMON_BASE + 0x4000) /* Internal Tx buffer address of the iinchip */
#define __DEF_IINCHIP_MAP_RXBUF__ (COMMON_BASE + 0x6000) /* Internal Rx buffer address of the iinchip */


#if (__MCU_TYPE__ == __MCU_AVR__)
   #ifdef __DEF_IINCHIP_INT__
      // iinchip use external interrupt 4
      #define IINCHIP_ISR_DISABLE()	(EIMSK &= ~(0x10))
      #define IINCHIP_ISR_ENABLE()	(EIMSK |= 0x10)
      #define IINCHIP_ISR_GET(X)		(X = EIMSK)
      #define IINCHIP_ISR_SET(X)		(EIMSK = X)
   #else
      #define IINCHIP_ISR_DISABLE()
      #define IINCHIP_ISR_ENABLE()	
      #define IINCHIP_ISR_GET(X)
      #define IINCHIP_ISR_SET(X)
   #endif
#else
#error "unknown MCU type"
#endif


#define SOCKET_REG_SIZE    0x40     // SOCKET Regsiter Count per Channel

/* gcc version */
/* WinAVR-20050214-install.exe */
#define __WINAVR_20050214__	0
#define __WINAVR_20060125__	1
#define __WINAVR_20060421__	2


#define __W5100__				0
#define __W5300__				1

#define __IINCHIP_TYPE__		__W5300__


/* #define __COMPILER_VERSION__	__WINAVR_20060421__ // <- move makefile*/
#define __COMPILER_VERSION__	__WINAVR_20060421__




#if (__COMPILER_VERSION__ == __WINAVR_20050214__)
#ifndef __STDIO_FDEVOPEN_COMPAT_12 
#define __STDIO_FDEVOPEN_COMPAT_12
#endif
#endif

#ifndef NULL
#define NULL		((void *) 0)
#endif

typedef enum { cfalse, ctrue } cbool;

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

typedef unsigned int uint;

/**
 * The 8-bit signed data type.
 */
typedef char int8;
/**
 * The volatile 8-bit signed data type.
 */
typedef volatile char vint8;
/**
 * The 8-bit unsigned data type.
 */
typedef unsigned char uint8;
/**
 * The volatile 8-bit unsigned data type.
 */
typedef volatile unsigned char vuint8;

/**
 * The 16-bit signed data type.
 */
typedef int int16;
/**
 * The volatile 16-bit signed data type.
 */
typedef volatile int vint16;
/**
 * The 16-bit unsigned data type.
 */
typedef unsigned int uint16;
/**
 * The volatile 16-bit unsigned data type.
 */
typedef volatile unsigned int vuint16;
/**
 * The 32-bit signed data type.
 */
typedef long int32;
/**
 * The volatile 32-bit signed data type.
 */
typedef volatile long vint32;
/**
 * The 32-bit unsigned data type.
 */
typedef unsigned long uint32;
/**
 * The volatile 32-bit unsigned data type.
 */
typedef volatile unsigned long vuint32;

/* bsd */
typedef uint8			u_char;		/**< 8-bit value */
typedef uint8 			SOCKET;
typedef uint16			u_short;	/**< 16-bit value */
typedef uint16			u_int;		/**< 16-bit value */
typedef uint32			u_long;		/**< 32-bit value */

typedef union _un_l2cval {
	u_long	lVal;
	u_char	cVal[4];
}un_l2cval;

typedef union _un_i2cval {
	u_int	iVal;
	u_char	cVal[2];
}un_i2cval;


/** global define */
#define FW_VERSION		0x01000000	/**< System F/W Version(test) : 0.0.0.1	*/
#define HW_VERSION	       0x01000000


#define TX_RX_MAX_BUF_SIZE	2048
#define TX_BUF	0x1100
#define RX_BUF	(TX_BUF+TX_RX_MAX_BUF_SIZE)

#define UART_DEVICE_CNT		1	/**< UART device number */
#define SUPPORT_UART_ONE

#define getSockStatus			getSn_SR
#define getRecvSize				getSn_RX_RSR

// Enter interrupt	
#define sei()  __asm__ __volatile__ ("sei" ::)
#define cli() __asm__ __volatile__ ("cli" ::)
typedef prog_char code_area;
#endif		/* _TYPE_H_ */
