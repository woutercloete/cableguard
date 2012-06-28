/****************************************************************************************/
#ifndef STORAGE_H_
#define STORAGE_H_
/****************************************************************************************/
#include <string.h>
#include <math.h>
#include <util/atomic.h>
#include <avr/eeprom.h>
/****************************************************************************************/
#include "payload.h"
#include "at45db041.h"
/****************************************************************************************/
#define FLASH_MAGIC_NUM_0     0xA1B2C100
#define EEPROM_MAGIC_NUM_0    0xA1B2C300
/****************************************************************************************/
#define WOUTER_DRAXIN
#undef PIETER_HUIS
#undef PIETER_DRAXIN
/****************************************************************************************/
using namespace XREADER;
/****************************************************************************************/
const u16 configBasePage = 0;
const u16 swBasePage = 1;
const u32 swSizeBytes = ((u16) 128 * (u16) 1024);
const u16 swSizePage = ceil(swSizeBytes / pageSize);
const u32 swTopPage = ((u16) swBasePage + (u16) swSizePage - 1);
const u08 swBlocksPerPage = floor(pageSize / sizeof(sSwBlock));
/****************************************************************************************/
#ifdef WOUTER_DRAXIN
const sIP defServerIP = {192, 168, 15, 74};
const sIP defReaderIP = {192, 168, 15, 210};
#else
const sIP defServerIP = { 10, 0, 0, 252 };
const sIP defReaderIP = { 10, 0, 0, 250 };
#endif
const sIP defNetMask = { 255, 255, 255, 0 };
const sIP defDns = { 10, 0, 0, 1 };
const sIP defGateway = { 10, 0, 0, 1 };
const u16 defServerPort = 7667;
const sMacADR defMacAdr = { 0x00, 0x00, 0x00, 0xDC, 0x08, 0x00 };
const sDate defAutoTime = { 0, 0, 0, 4, 0, 0 };
const u16 bufSize = (SW_BLOCK_SIZE * 2);
/****************************************************************************************/
class Cstorage {
  public:
    struct {
        u32 magic;
        bool networkConfigured;
        bool readerConfigured;
        sConfig config;
    } eeprom;
    struct {
        u32 magic;
        sSwMeta currentSw;
        sSwMeta storedSw;
    } flash;
    sNetConfig dhcpConfig;
    Cat45db041* flashdrv;
    Cstorage(Cat45db041* flash);
    u08 getRSSIThreshold(){
      return eeprom.config.rssiReject;
    }
    void setStoredSwMeta(sSwMeta meta);
    void setCurrentSwMeta(sSwMeta meta);
    void setNetConf();
    void setConfig(sConfig config);
    void setReaderID(u32 _readerID);
    void setMacAdr(sMacADR adr);
    void setServerConfig(sServerConfig _serverIP);
    void setNetConfig(sNetConfig netConfig);
    void eraseFlash(void);
    void eraseEeprom(void);
    void eraseSw();
    void setSwBlock(sSwBlock* block);
    u32 getReaderID(void) {
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        eeprom_read_block(&eeprom, 0, sizeof(eeprom));
        return eeprom.config.readerID;
      }
    }
    void getSwBlock(sSwBlock* block);
    void calcSwBlockCRC(sSwBlock* block, u16* crc);
    void calcSwCRC(u16* crc, sSwMeta meta);
};
/****************************************************************************************/

#endif /* STORAGE_H_ */
