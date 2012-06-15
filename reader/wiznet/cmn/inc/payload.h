/****************************************************************************************/
// The file contains the definitions needed to communicate with the server.
//
//   Created : 15 March 2012
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
/****************************************************************************************/
#ifndef PAYLOAD_H
#define PAYLOAD_H
/****************************************************************************************/
#include "avrlibtypes.h"
#include "tag.h"
#include "MCP7940x.h"
/****************************************************************************************/
#define STATE_UNKNOWN 0
#define MAX_RSP_SIZE  128
#define SW_BLOCK_SIZE 64
/****************************************************************************************/
#define MARKER          0xA0B1C2D4
#define EVENT_VERSION   857
#define TAG_VERSION     857
/****************************************************************************************/
namespace XREADER {
  /****************************************************************************************/
  typedef enum {
    GET_DATE = 0x00,
      SET_STATUS = 0x01,
      SET_EVENT = 0x02,
      GET_CONFIG = 0x03,
      GET_SW_BLOCK = 0x04,
      GET_READER_ID = 0x05,
      GET_MAC_ADR = 0x06,
      GET_DST_CONFIG = 0x07,
      GET_NET_CONFIG = 0x08,
      SET_EVENT_02 = 0x09,
  } eOpcodes;
  /****************************************************************************************/
  typedef enum {
    B_FALSE = 0x00, B_TRUE = 0x01
  } eBool;
  /****************************************************************************************/
  typedef sRtcTime sDate;
  /****************************************************************************************/
  typedef struct {
      u32 tagID;
      u16 lifecnt;
      u08 movement;
  } sServerTag01;
  /****************************************************************************************/
  typedef struct {
      u08 mac0;
      u08 mac1;
      u08 mac2;
      u08 mac3;
      u08 mac4;
      u08 mac5;
  } sMacADR;
  /****************************************************************************************/
  typedef struct {
      u08 ip0;
      u08 ip1;
      u08 ip2;
      u08 ip3;
  } sIP;
  /****************************************************************************************/
  typedef struct {
      sIP src;
      sIP netMask;
      sIP gateway;
      sIP dns;
      eBool useDHCP;
  } sNetConfig;
  /****************************************************************************************/
  typedef struct {
      sServerTag01 tag;
      u32 readerID;
      TAG::eEvent eventType;
      sDate date;
  } sEvent01;
  /****************************************************************************************/
  typedef struct {
      TAG::sServerTag02 tag;
      u32 readerID;
      TAG::eEvent eventType;
      sDate date;
  } sEvent02;
  /****************************************************************************************/
  typedef struct {
      u16 revision;
      u16 numBlocks;
      u32 size;
      u16 blockSize;
      u16 crc;
  } sSwMeta;
  /****************************************************************************************/
  typedef struct {
      u16 blockRevision;
      u16 blockNumber;
      u16 blockSize;
      u16 blockCRC;
  } sSwBlockMeta;
  /****************************************************************************************/
  typedef struct {
      u32 readerID;
      u32 state;
      sSwMeta sw;
      sDate autoConnectTime;
  } sStatus;
  /****************************************************************************************/
  typedef struct {
      sIP ip;
      u16 port;
  } sServerConfig;
  /****************************************************************************************/
  typedef struct {
      u08 adr;
      u08 channel;
  } sRfConfig;
  /****************************************************************************************/
  typedef struct {
      sSwMeta sw;
      sDate autoConnectTime;
      u32 readerID;
      sRfConfig rf;
      sMacADR macAdr;
      sServerConfig serverConfig;
      sNetConfig netConfig;
      u16 rssiReject;
  } sConfig;
  /****************************************************************************************/
  typedef struct {
      sSwBlockMeta meta;
      u08 dat[SW_BLOCK_SIZE];
  } sSwBlock;
  /****************************************************************************************/
  typedef struct {
      eOpcodes Opcode;
      u32 readerID;
      union {
          sEvent01 event;
          sEvent02 event02;
          sStatus status;
          sSwBlockMeta meta;
      };
  } sCmd;
  /***************************  RESPONSE STRUCTURES ***************************************/
  typedef struct {
      eOpcodes Opcode;
      bool ack;
      union {
          union {
              sDate date;
              sConfig config;
              u32 readerID;
              sMacADR mac;
              sServerConfig server;
              sNetConfig netConfig;
          };
          struct {
              sSwBlockMeta meta;
              u08 dat[SW_BLOCK_SIZE];
          };
      };
  } sResp;
/****************************************************************************************/
}
#endif
