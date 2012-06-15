#ifndef	_SOCKET_H_
#define	_SOCKET_H_

#include "types.h"

#if (__IINCHIP_TYPE__ == __W5100__)
 #include "w5100.h"
#elif (__IINCHIP_TYPE__ == __W5300__)
 #include "w5300.h"
#else
	#error "unknown iinchip type"
#endif

/**********************************************************
*
* define function of socket API 
*
***********************************************************/



uint8 socket(SOCKET s, uint8 protocol, uint16 port, uint16 flag); // Opens a socket(TCP or UDP or IP_RAW mode)
void closeSocket(SOCKET s); // Close socket
uint8 connectSocket(SOCKET s, uint8 * addr, uint16 port); // Establish TCP connection (Active connection)
void disconnectSocket(SOCKET s); // disconnect the connection
uint8 listenSocket(SOCKET s);	// Establish TCP connection (Passive connection)
uint32 sendSocket(SOCKET s, uint8 * buf, uint32 len); // Send data (TCP)
uint32 recvSocket(SOCKET s, uint8 * buf, uint32 len);	// Receive data (TCP)
uint16 sendtoSocket(SOCKET s, uint8 * buf, uint16 len, uint8 * addr, uint16 port); // Send data (UDP/IP RAW)
uint16 recvfromSocket(SOCKET s, uint8 * buf, uint16 len, uint8 * addr, uint16  *port); // Receive data (UDP/IP RAW)
uint16 igmpsendSocket(SOCKET s, uint8 * buf, uint16 len); // Send data (multicasting)
#endif
/* _SOCKET_H_ */
