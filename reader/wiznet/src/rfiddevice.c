//=============================================================================
#include <string.h>
#include "global.h"
//=============================================================================
#include "main.h"
#include "timer1280.h"
#include "uart4.h"
#include "printf.h"
//=============================================================================
#include "drv_rfidpacket.h"
#include "rfiddevice.h"
#include "CServer.h"
//=============================================================================
#define TAG_TABLE_SIZE    10
#define RFID_TAG_LEN      11
#define PERSIST_TOP       400
//=============================================================================
#define RFID_RX_TIMEOUT   30
//=============================================================================
#define SEND_CMD          0
#define WAIT_RESP         1
//=============================================================================
#define RFID_UART         1

//=============================================================================
typedef struct {
  u08 ID;
  u08 RSSI;
  u08 CustomerCode;
  u08 Mode;
  u08 LowVoltageLevel;
  u08 IO;
  u08 LongRangeTx;
  u08 AlarmsMode;
  u08 RF_OnOff;
  u08 RSSI_Offset;
  u08 BufferSize;
  u08 BufferTimeout;
  u08 SerialNumber;
  u08 DuressOffset;
  u08 TagDataEnable;
  u08 TimeDate;
} sRFIDReceiver;
//======================================================================
typedef struct {
  u16 persistcnt;
  u08 RfidTag[RFID_TAG_LEN];
  u08 status;
} sRfidTagEntry;
//=============================================================================
static sRFIDReceiver oRfidReceiver;
static sRfidTagEntry RfidTagTable[TAG_TABLE_SIZE];
extern u08 RfidPacket[RFID_STXETX_MAXRXPACKETSIZE];

u08 rxState;
u16 rxTimeout;
u16 missCnt;
u16 hitCnt;
u16 serviceCnt;
u16 timeoutCnt;
u08 debugLevel;
bool manualMode;

//=============================================================================
void InitRFID(void) {
  Drv_RfidPacketInit(uart1SendByte);
  oRfidReceiver.ID = 1;
  oRfidReceiver.RSSI = 0;
  oRfidReceiver.CustomerCode = 1;
  oRfidReceiver.Mode = 0;
  oRfidReceiver.LowVoltageLevel = 1;
  oRfidReceiver.IO = 0;
  oRfidReceiver.LongRangeTx = 0;
  oRfidReceiver.AlarmsMode = 0;
  oRfidReceiver.RF_OnOff = 0;
  oRfidReceiver.RSSI_Offset = 0;
  oRfidReceiver.BufferSize = 0;
  oRfidReceiver.BufferTimeout = 0;
  oRfidReceiver.SerialNumber = 0;
  oRfidReceiver.DuressOffset = 0;
  oRfidReceiver.TagDataEnable = 0;
  oRfidReceiver.TimeDate = 0;
  memset(RfidTagTable, 0x00, sizeof(sRfidTagEntry));
  debugLevel = 1;
  if (debugLevel == 1) {
    missCnt = 0;
    hitCnt = 0;
    serviceCnt = 0;
  }
  rxState = SEND_CMD;

  manualMode = false;
}

//=============================================================================
bool AddNewInTable(u08 *pRfidTag)
{
  u08 i;
  u16 TagNumber;
  TagNumber = GetTagNumber(pRfidTag);

  if(TagNumber!=0)
  {
	  for (i = 0; i < TAG_TABLE_SIZE; i++) {
		//Get Empty slot
		if (RfidTagTable[i].persistcnt == 0)
		{
	#if (MODEM_PRESENT==1)
			if (Server.SetTagInRange(GetTagNumber(pRfidTag)))
	#else
		   if (1)
	#endif
		  {
			RfidTagTable[i].status = TAG_PRESENT;
			RfidTagTable[i].persistcnt = PERSIST_TOP;
			memcpy(RfidTagTable[i].RfidTag, pRfidTag, RFID_TAG_LEN);
			if (debugLevel == 1)
			  PRINTF("\r\nADDNEW[%d]",GetTagNumber(pRfidTag));
			return true;
		  }
		}
	  }
  }
  return false;
}

//=============================================================================
u08 UpdateTagInTable(u08 *pRfidTag) {
  u08 i;
  u16 TagNumber;
  u16 TableTagNumber;
  u08 TagDuress;

  // Get current received tags ID Number
  TagNumber = GetTagNumber(pRfidTag);
  PRINTF("\r\nTagSeen=%d",TagNumber);
  TagDuress = GetTagFlags(pRfidTag);
  for (i = 0; i < TAG_TABLE_SIZE; i++) {
    TableTagNumber = GetTagNumber(RfidTagTable[i].RfidTag);
    if ((TagNumber == TableTagNumber) && TagNumber != 0 && (RfidTagTable[i].status != TAG_LOST)) //search for tag in table
    {
      RfidTagTable[i].persistcnt = PERSIST_TOP;
      if (TagDuress) {
        RfidTagTable[i].status = TAG_DURESS;
        if (debugLevel == 1)
          PRINTF("\r\n ## DURESS ### ");
#if (MODEM_PRESENT==1)
        Server.SetTagDuress(GetTagNumber(pRfidTag));
#endif
      } else {
        if (RfidTagTable[i].status != TAG_DURESS) {
          RfidTagTable[i].status = TAG_PRESENT;
        }
      }
      return true;
    }
  }
  return false;
}
//==============================================================================
// AA 0F 01 11   91 00 14 00 00 06 7C 00 10 00 32   2C 44
//               9F 00 14 00 00 09 D0 00 10 00 B7   9B
//==============================================================================
/*
u08 GetTagPacket(sRFIDReceiver *oRfidReceiver, u08 *pRfidTag) {
  if (manualMode)
    Drv_RfidPacketSend(0x04, oRfidReceiver->ID, GET_TAG_PACKET,0);
  if (Drv_RfidPacketProcess(uartGetRxBuffer(RFID_UART))) {
    memcpy(pRfidTag, &RfidPacket[4], 11);
  } else {
    return false;
  }
  return true;
}
*/
//=============================================================================
void ServiceRFID(void) {
  u08 RfidTag[RFID_TAG_LEN];
  u08 i;
  if (debugLevel == 1)
    serviceCnt++;

  switch (rxState) {
  case SEND_CMD:
    if (manualMode)
      Drv_RfidPacketSend(0x04, oRfidReceiver.ID, GET_TAG_PACKET,0);
    rxState = WAIT_RESP;
    rxTimeout = 0;
    break;
  case WAIT_RESP:
    rxTimeout++;
    if (debugLevel == 1)
      missCnt++;

    while (Drv_RfidPacketProcessNew(uartGetRxBuffer(RFID_UART))) {
      memcpy(&RfidTag, &RfidPacket[4], 11);
      if (!(UpdateTagInTable(RfidTag)))
      {
    	  PRINTF("\n\r Not In Table");
    	  if (!(AddNewInTable(RfidTag)))
    	  {
    		  PRINTF("\n\r!!Table Full !!");
    	  }
      }
      if (debugLevel == 1)
        hitCnt++;
      missCnt--;
      if (manualMode)
        Drv_RfidPacketSend(0x04, oRfidReceiver.ID, GET_TAG_PACKET,0);
      rxTimeout = 0;
    }
    if (rxTimeout >= RFID_RX_TIMEOUT) {
      if (debugLevel == 1)
        timeoutCnt++;
      //rxState = SEND_CMD;
    }
    break;
  default:
    rxState = SEND_CMD;
    break;
  }
  if (debugLevel == 2) {
    if (serviceCnt % 10 == 1) {
      PRINTF("\rIn serviceRFID : services  %u hits %u misses %u timeouts %u", serviceCnt, hitCnt, missCnt, timeoutCnt);
    }
  }
}
//=============================================================================
void ServiceTagTable(void) {
  u08 i;
  u16 TagNumber;

  if (debugLevel == 1)
    PRINTF("\r\nTAGS :");
  for (i = 0; i < TAG_TABLE_SIZE; i++) {
    TagNumber = GetTagNumber(RfidTagTable[i].RfidTag);
    if (debugLevel == 1) {
      if (RfidTagTable[i].persistcnt > 10) {
        PRINTF("%d[%d] ",TagNumber,RfidTagTable[i].persistcnt);
      } else if (RfidTagTable[i].persistcnt <= 10 && RfidTagTable[i].persistcnt
          > 1) {
        PRINTF("\t%d %d #WARN#",TagNumber,RfidTagTable[i].persistcnt);
      } else if (RfidTagTable[i].persistcnt == 1) {
        PRINTF("\t\t%d %d #LOST#",TagNumber,RfidTagTable[i].persistcnt);
      }
    }
    // decrement persist counter - tag not seen but was in table
    if (RfidTagTable[i].persistcnt > 1) {
      // decrement persist counter - tag not seen but was in table
      RfidTagTable[i].persistcnt--;
    } else if (RfidTagTable[i].persistcnt == 1) // Tag Lost
    {
#if (MODEM_PRESENT==1)
    	if(Server.SetTagOutRange(TagNumber))
#else
    	if (1)
#endif
      {
        RfidTagTable[i].status = TAG_LOST;
        RfidTagTable[i].persistcnt = 0;
      }
    }
  }
}
//=============================================================================
TagStateRFID GetTagStateRFID(u32 RFID) {
  TagStateRFID State;
  u08 i;
  u32 TagNumber;

  State = TR_GONE;

  for (i = 0; i < TAG_TABLE_SIZE; i++) {
    TagNumber = GetTagNumber(RfidTagTable[i].RfidTag);
    if ((TagNumber == RFID)) //search for tag in table
    {
      if (RfidTagTable[i].status == TAG_PRESENT) {
        State = TR_SEEN;
      } else if (RfidTagTable[i].status == TAG_DURESS) {
        RfidTagTable[i].status = TAG_PRESENT;
        State = TR_DURESS;
      }
      if (debugLevel == 1) {
        PRINTF("\r\nTagState %u ", State);
        PRINTF("RFID %u", RFID);
        PRINTF(" TagNumber %u", TagNumber);
        PRINTF("\r\n", TagNumber);
      }
    }
  }
  return State;
}

