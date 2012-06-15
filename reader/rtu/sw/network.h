#ifndef CNETWORK_H_
#define CNETWORK_H_
/****************************************************************************************/
#include "types.h"
#include "uart.h"
#include "crc.h"
/****************************************************************************************/
namespace NETWORK {
  const u08 NUM_PAYLOAD_BYTES = 128;
  const u08 HEADER = 0x5A;
  const u08 BROADCAST_NODE_ID = 0x0;
  typedef struct {
      u08 Header;
      u08 Size;
      u08 NotSize;
      u08 DstNode;
      u08 SrcNode;
      u08 TransactNum;
      u08 CRC;
  } sHeader;
  typedef enum {
    STATE_RX_HEADER,
      STATE_RX_SIZE,
      STATE_RX_NOT_SIZE,
      STATE_RX_DST_NODE,
      STATE_RX_SRC_NODE,
      STATE_RX_TRANSACT_NUM,
      STATE_RX_CRC,
      STATE_RX_PAYLOAD,
      STATE_SKIP_PAYLOAD,
      STATE_PACKET_AVAILABLE
  } eState;
  class CNetwork {
    private:
      Cuart* UART;
      u08 cntByte;
      u32 timeLimit;
      u16 baudRate;
      u08 payloadSize;
    public:
      eState state;
      u08 nodeId;
      u08 healthy;
      volatile u32 time;
      sHeader Header;
      u08* payload;
      CNetwork(Cuart* UART, u08 size, u08 node = 0) {
        this->UART = UART;
        this->nodeId = node;
        state = STATE_RX_HEADER;
        time = 0;
        timeLimit = 0;
        payloadSize = 0;
        payload = 0;
        healthy = true;
        baudRate = this->UART->baudRate;
        setPayloadBufSize(size);
      }
      u08 setPayloadBufSize(u08 size) {
        payload = (u08*) malloc(size);
        if (payload == NULL) {
          healthy = false;
          return false;
        }
        payloadSize = size;
        return true;
      }
      void service(void) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
          if (time > timeLimit) {
            state = STATE_RX_HEADER;
            time = 0;
          }
        }
        switch (state) {
          case STATE_RX_HEADER:
            if (UART->receive(&Header.Header, 1) == 1) {
              if (Header.Header == HEADER) {
                cntByte = 0;
                state = STATE_RX_SIZE;
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                  time = 0;
                }
                timeLimit = 2 * (sizeof(sHeader) * 10 * 1000000) / (baudRate);
              }
            }
            break;
          case STATE_RX_SIZE:
            if (UART->receive(&Header.Size, 1) == 1) {
              state = STATE_RX_NOT_SIZE;
            }
            break;
          case STATE_RX_NOT_SIZE:
            if (UART->receive(&Header.NotSize, 1) == 1) {
              if ((Header.Size ^ Header.NotSize) == 0xFF) {
                state = STATE_RX_DST_NODE;
              } else {
                state = STATE_RX_HEADER;
              }
            }
            break;
          case STATE_RX_DST_NODE:
            if (UART->receive(&Header.DstNode, 1) == 1) {
              state = STATE_RX_SRC_NODE;
            }
            break;
          case STATE_RX_SRC_NODE:
            if (UART->receive(&Header.SrcNode, 1) == 1) {
              state = STATE_RX_TRANSACT_NUM;
            }
            break;
          case STATE_RX_TRANSACT_NUM:
            if (UART->receive(&Header.TransactNum, 1) == 1) {
              state = STATE_RX_CRC;
            }
            break;
          case STATE_RX_CRC:
            if (UART->receive(&Header.CRC, 1) == 1) {
              cntByte = 0;
              ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                time = 0;
              }
              timeLimit = 2 * (Header.Size * 10 * 1000000) / (baudRate);
              if (Header.DstNode == nodeId || Header.DstNode == BROADCAST_NODE_ID) {
                state = STATE_RX_PAYLOAD;
              } else {
                state = STATE_SKIP_PAYLOAD;
              }
            }
            break;
          case STATE_SKIP_PAYLOAD:
            if (UART->receive(&payload[0], 1) == 1) {
              cntByte++;
              if (cntByte == Header.Size) {
                state = STATE_RX_HEADER;
              }
            }
            break;
          case STATE_RX_PAYLOAD:
            if (UART->receive(&payload[cntByte], 1) == 1) {
              cntByte++;
              if (cntByte == Header.Size) {
                u08 calcCrc;
                calcCrc = crc8((u08*) payload, Header.Size);
                if ((calcCrc == Header.CRC)
                    && (Header.DstNode == nodeId || Header.DstNode == BROADCAST_NODE_ID)) {
                  state = STATE_PACKET_AVAILABLE;
                  //timeLimit = 500000;
                } else {
                  state = STATE_RX_HEADER;
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
            state = STATE_RX_HEADER;
            break;
        }
      }
      void reset(void) {
        state = STATE_RX_HEADER;
      }
      u08 packetAvailable(void) {
        if (state == STATE_PACKET_AVAILABLE) {
          return true;
        }
        return false;
      }
      void tx(u08 transactNum, u08 dstNode, u08* Dat, u08 byteCnt) {
        sHeader Header;
        Header.Header = HEADER;
        Header.Size = byteCnt;
        Header.NotSize = (byteCnt ^ 0xFF);
        Header.DstNode = dstNode;
        Header.SrcNode = nodeId;
        Header.TransactNum = transactNum;
        Header.CRC = crc8(Dat, byteCnt);
        UART->send((c08*) &Header, sizeof(Header));
        UART->send((c08*) Dat, byteCnt);
      }
  };
}
/****************************************************************************************/
using namespace NETWORK;
/****************************************************************************************/
#endif
