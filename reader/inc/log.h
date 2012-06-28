/*
 * CLog.h
 *
 *  Created on: 07 Feb 2010
 *      Author: Wouter
 */

#ifndef CLOG_H_
#define CLOG_H_

#include "logmessage.h"
#include "CFram.h"
#include "MCP7940x.h"

#define BLOCK_HEADER 0xA5


typedef struct {
    u08 header;
    u32 time;
    u32 writeAdr;
    u08 crc;
} sAllocationBlock;

typedef struct {
    sAllocationBlock blockA;
    sAllocationBlock blockB;
} sAllocationTable;

class CLog {
  private:
	Crtc* rtc;
    sAllocationBlock validBlock;
    u16 blockAdr;
    void updateAllocation(u16 writeAdr, u32 _time);
    void saveAllocationBlock(u16 blockAdr, sAllocationBlock* block);
    sAllocationTable loadAllocationTable();
    sAllocationBlock getValidBlock(void);
    u32 getWriteAdr(void);
    void write(u08* dat, u16 size, u32 time);
  public:
    CFram* fram;
    CLog(CFram* _fram, Crtc* _rtc);
    void Init(void);
    void logMsgEvent(eLogMessage event);
    void logSafeSelected(u08 safeNumber);
    void logCodeEntered(u32 code);
    void logVehicleNumberChanged(u16 vehNumber);
    void logTimeChanged(s08 mintuesAdvanced, s08 hoursAdvanced);
    void saveAllocationTable(sAllocationTable* table);
    void resetAllocationTable(sAllocationTable* table);
};

#endif /* CLOG_H_ */
