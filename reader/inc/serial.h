/*
*
@file		serial.h
@brief	Header file for ATmega64 UART. (AVR-GCC Compiler)
*
*/

#ifndef _SERIAL_H_
#define _SERIAL_H_

#define MAX_SIO_COUNT		32


#define SUPPORT_UART_ONE

#define UART_BAUD_RATE(X)	((X==0) ? 2400 : 	\
				 (X==1) ? 4800 : 	\
				 (X==2) ? 9600 : 	\
				 (X==3) ? 14400 : 	\
				 (X==4) ? 19200 : 	\
				 (X==5) ? 28800 : 	\
				 (X==6) ? 38400 :	\
				 (X==7) ? 57600 : 	\
				 (X==8) ? 76800 :	\
				 (X==9) ? 115200 :	\
				 (X==10) ? 250000 :	\
				 (X==11) ? 500000 :	\
				 1000000)


#define UART_BAUD_SELECT(X) 	(u_int)((float)CLK_CPU/(float)(UART_BAUD_RATE(X)*16) -0.5f)


#define XON_CHAR	0x11
#define XOFF_CHAR	0x13


/*
********************************************************************************
Function Prototype Definition Part
********************************************************************************
*/
extern void uart_init(u_char uart, u_char baud_index);		/* Initialize the UART of ATmega64 */

extern void uart_databit(u_char uart, u_char dbit);

extern void uart_stopbit(u_char uart, u_char sbit);

extern void uart_paritybit(u_char uart, u_char pbit);

extern void uart_flowctrl(u_char uart, u_char flow);

extern u_int uart_keyhit(u_char uart);	/* Check if there is Rx */

extern void uart_puts(u_char uart, char * str);	/* Send a string to UART */
extern int uart_gets(u_char uart, char * str,char bpasswordtype, int max_len);	/* Get a string from UART */
extern void uart_flush_rx(u_char uart);	/* Flush RX Buffer */



#endif		/* _SERIAL_H_ */

