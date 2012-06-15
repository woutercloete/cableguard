/****************************************************************************************/
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <util/atomic.h>
#include "iopins.h"
#else
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include "main.h"
#define ATOMIC_BLOCK(val)
#endif
/****************************************************************************************/
#include "CUART.h"
#include "crc.h"
#include "avrlibtypes.h"
#include "network.h"
/****************************************************************************************/
using namespace CNETWORK;
/****************************************************************************************/
#undef DEBUG
/****************************************************************************************/
#ifndef WIN32

#define EE_NODEID			(u08 *)(0x40)

#else
AnsiString str;
#endif
/****************************************************************************************/
CNetwork::CNetwork(CUART* UART, u08 size) {
	this->UART = UART;
	this->NodeId = nodeidGet();
	State = STATE_RX_HEADER;
	time = 0;
	timeLimit = 0;
	payloadSize = 0;
	payload = 0;
	healthy = true;
	baudRate = this->UART->baudRate;
	setPayloadBufSize(size);
}
/****************************************************************************************/
CNetwork::CNetwork(CUART* UART, u08 size, u08 node) {
	this->UART = UART;
	nodeidSet(node);
	this->NodeId = node;
	State = STATE_RX_HEADER;
	time = 0;
	timeLimit = 0;
	payloadSize = 0;
	payload = 0;
	healthy = true;
	baudRate = this->UART->baudRate;
	setPayloadBufSize(size);
}
/****************************************************************************************/
u08 CNetwork::setPayloadBufSize(u08 size) {
	payload = (u08*) malloc(size);
	if (payload == NULL) {
		healthy = false;
		return false;
	}
	payloadSize = size;
	return true;
}
/****************************************************************************************/
void CNetwork::service(void) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		if (time > timeLimit) {
			State = STATE_RX_HEADER;
			time = 0;
		}
	}
	switch (State) {
	case STATE_RX_HEADER:
		if (UART->receive(&Header.Header, 1) == 1) {
			if (Header.Header == HEADER) {
				cntByte = 0;
				State = STATE_RX_SIZE;
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
					time = 0;
				}
				timeLimit = 2 * (sizeof(sHeader) * 10 * 1000000) / (baudRate);
			}
		}
		break;
	case STATE_RX_SIZE:
		if (UART->receive(&Header.Size, 1) == 1) {
			State = STATE_RX_NOT_SIZE;
		}
		break;
	case STATE_RX_NOT_SIZE:
		if (UART->receive(&Header.NotSize, 1) == 1) {
			if ((Header.Size ^ Header.NotSize) == 0xFF) {
				State = STATE_RX_DST_NODE;
			} else {
				State = STATE_RX_HEADER;
			}
		}
		break;
	case STATE_RX_DST_NODE:
		if (UART->receive(&Header.DstNode, 1) == 1) {
			State = STATE_RX_SRC_NODE;
		}
		break;
	case STATE_RX_SRC_NODE:
		if (UART->receive(&Header.SrcNode, 1) == 1) {
			State = STATE_RX_TRANSACT_NUM;
		}
		break;
	case STATE_RX_TRANSACT_NUM:
		if (UART->receive(&Header.TransactNum, 1) == 1) {
			State = STATE_RX_CRC;
		}
		break;
	case STATE_RX_CRC:
		if (UART->receive(&Header.CRC, 1) == 1) {
			cntByte = 0;
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				time = 0;
			}
			timeLimit = 2 * (Header.Size * 10 * 1000000) / (baudRate);
			if (Header.DstNode == NodeId || Header.DstNode == BROADCAST_NODE_ID) {
				State = STATE_RX_PAYLOAD;
			} else {
				State = STATE_SKIP_PAYLOAD;
			}
		}
		break;
	case STATE_SKIP_PAYLOAD:
		if (UART->receive(&payload[0], 1) == 1) {
			cntByte++;
			if (cntByte == Header.Size) {
				State = STATE_RX_HEADER;
			}
		}
		break;
	case STATE_RX_PAYLOAD:
		if (UART->receive(&payload[cntByte], 1) == 1) {
			cntByte++;
			if (cntByte == Header.Size) {
				u08 calcCrc;
				calcCrc = crc8((u08*) payload, Header.Size);
				if ((calcCrc == Header.CRC) && (Header.DstNode == NodeId
						|| Header.DstNode == BROADCAST_NODE_ID)) {
					State = STATE_PACKET_AVAILABLE;
					//timeLimit = 500000;
				} else {
					State = STATE_RX_HEADER;
				}
			}
		}
		break;
	case STATE_PACKET_AVAILABLE:
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			time = 0;
		}
		break;
	default:
		State = STATE_RX_HEADER;
		break;
	}
}
/****************************************************************************************/
void CNetwork::reset(void) {
	State = STATE_RX_HEADER;
}
/****************************************************************************************/
u08 CNetwork::packetAvailable(void) {
	if (State == STATE_PACKET_AVAILABLE) {
		return true;
	}
	return false;
}
/****************************************************************************************/
void CNetwork::tx(u08 transactNum, u08 dstNode, u08* Dat, u08 byteCnt) {
	//LED2_TOGGLE();
	//u08* pkt;
	sHeader Header;
	//pkt = (u08*) malloc(byteCnt+sizeof(Header));
	Header.Header = HEADER;
	Header.Size = byteCnt;
	Header.NotSize = (byteCnt ^ 0xFF);
	Header.DstNode = dstNode;
	Header.SrcNode = NodeId;
	Header.TransactNum = transactNum;
	Header.CRC = crc8(Dat, byteCnt);
	UART->send((c08*) &Header, sizeof(Header));
	UART->send((c08*) Dat, byteCnt);
	//memcpy(pkt,(u08*) &Header, sizeof(Header));
	//memcpy(pkt+sizeof(Header),Dat, byteCnt);
	//UART->send((c08*)pkt, (byteCnt+sizeof(Header)));
	//free(pkt);

}
/****************************************************************************************/
#define EE_MAX	4

u08 CNetwork::nodeidGet(void) {
#ifdef WIN32
	return MASTER_NODE_ID;
#else
	u08 block[EE_MAX];
	u08 i, j, big, id;
	u08 cnt[EE_MAX];

	memset(cnt, 0, EE_MAX);

	cli();
	for (i = 0; i < EE_MAX; i++) {
		block[i] = eeprom_read_byte(EE_NODEID + i);
	}

	for (i = 0; i < EE_MAX; i++) {
		for (j = 0; j < EE_MAX; j++) {
			if (block[j] == block[i])
				cnt[i]++;
		}
	}
	id = 0;
	big = 0;
	for (i = 0; i < EE_MAX; i++) {
		if (cnt[i] > big) {
			big = cnt[i];
			id = block[i];
		}
	}

	for (i = 0; i < EE_MAX; i++) {
		/* reset the node ID assume less than five means a corrupt value */
		if (cnt[i] < 5) {
			eeprom_write_byte((EE_NODEID) + i, id);
		}
	}
	sei();

	return id;
#endif
}
/****************************************************************************************/
void CNetwork::nodeidSet(u08 ID) {
#ifdef WIN32
	NodeId = MASTER_NODE_ID;
#else
	u08 i;
	if (ID < 100 || ID == UNCONF_NODE_ID) {
		this->NodeId = ID;
	} else {
		return;
	}
	cli();
	for (i = 0; i < 32; i++) {
		eeprom_write_byte((EE_NODEID) + i, ID);
	}
	sei();

#endif
}
/****************************************************************************************/
