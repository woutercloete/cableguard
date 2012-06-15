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
#define FLASH_MAGIC_NUM_1     0xA1B2C101
/****************************************************************************************/
#define EEPROM_MAGIC_NUM_0    0xA1B2C300
#define EEPROM_MAGIC_NUM_1    0xA1B2C301
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
const u08 numConfigs = 3;
const u08 defRssiThreshold = 250;
const sIP defServerIP[] = { { 172, 17, 49, 48 }, { 192, 168, 15, 74 }, { 10, 0, 0, 252 } };
const sIP defReaderIP[] =
    { { 172, 17, 49, 210 }, { 192, 168, 15, 210 }, { 10, 0, 0, 250 } };
const sIP defDns[] = { { 172, 17, 49, 20 }, { 192, 168, 15, 1 }, { 10, 0, 0, 1 } };
const sIP defGateway[] = { { 172, 17, 48, 2 }, { 192, 168, 15, 7 }, { 10, 0, 0, 1 } };
const eBool defDHCP[] = { B_TRUE, B_TRUE, B_TRUE };
const sIP defNetMask[] = { { 255, 255, 255, 0 }, { 255, 255, 255, 0 },
                           { 255, 255, 255, 0 } };
const u16 defServerPort = 7667;
const sMacADR defMacAdr = { 0x00, 0x00, 0x00, 0xDC, 0x08, 0x00 };
/****************************************************************************************/
const sDate defAutoTime = { 0, 0, 0, 4, 0, 0 };
const u16 bufSize = (SW_BLOCK_SIZE * 2);
/****************************************************************************************/
class Cstorage {
  public:
    struct {
        u32 magic;
        bool networkConfigured;
        bool readerConfigured;
        sConfig activeConfig;
        sConfig configs[numConfigs];
    } eeprom;
    struct {
        u32 magic;
        sSwMeta currentSw;
        sSwMeta storedSw;
    } flash;
    u08 activeConfigIndex;
    sNetConfig dhcpConfig;
    Cat45db041* flashdrv;
    Cstorage(Cat45db041* flash);
    bool incActiveConfig() {
      activeConfigIndex++;
      if (activeConfigIndex < numConfigs) {
        eeprom.activeConfig = eeprom.configs[activeConfigIndex];
        return true;
      }
      return false;
    }
    u08 getRSSIThreshold() {
      return eeprom.activeConfig.rssiReject;
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
      }
      return eeprom.activeConfig.readerID;
    }
    void getSwBlock(sSwBlock* block);
    void calcSwBlockCRC(sSwBlock* block, u16* crc);
    void calcSwCRC(u16* crc, sSwMeta meta);
};
/****************************************************************************************/

#endif /* STORAGE_H_ */
