/****************************************************************************************/
// This class contains all the visible tags and an event table.
//
//   Created : 15 March 2012
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
/****************************************************************************************/
#include <string.h>
/****************************************************************************************/
#include "storage.h"
#include "rfstudio.h"
#include "crc.h"
/****************************************************************************************/
Cstorage::Cstorage(Cat45db041* flashdrv) {
  u08 buf[pageSize];
  this->flashdrv = flashdrv;
  // Check if FLASH storage has been initialized
  flashdrv->read(buf, configBasePage);
  memcpy((u08*) &flash, buf, sizeof(flash));
  if (flash.magic != FLASH_MAGIC_NUM_0) {
#ifndef LOADER
    eraseFlash();
#endif
    activeConfigIndex = 0;
  }
  // Check if storage has been initilized.
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    eeprom_read_block(&eeprom, 0, sizeof(eeprom));
    if (eeprom.magic == EEPROM_MAGIC_NUM_1) {
#ifndef LOADER
      //eraseEeprom();
#endif
    }
    else {
#ifndef LOADER
      eraseEeprom();
#endif
    }
  }
}
/****************************************************************************************/
#ifndef LOADER
void Cstorage::setStoredSwMeta(sSwMeta meta) {
  u08 buf[pageSize];
  flashdrv->read(buf, configBasePage);
  memcpy(&flash, buf, sizeof(flash));
  flash.storedSw = meta;
  flashdrv->write(configBasePage, (u08*) &flash);
}
#endif
/****************************************************************************************/
void Cstorage::setCurrentSwMeta(sSwMeta meta) {
  u08 buf[pageSize];
  flashdrv->read(buf, configBasePage);
  memcpy(&flash, buf, sizeof(flash));
  flash.currentSw = meta;
  flashdrv->write(configBasePage, (u08*) &flash);
}
/****************************************************************************************/
void Cstorage::calcSwCRC(u16* crc, sSwMeta meta) {
  sSwBlock block;
  *crc = 0;
  u16 cnt;
  for (cnt = 0; cnt < meta.numBlocks; cnt++) {
    block.meta.blockNumber = cnt;
    block.meta.blockCRC = *crc;
    calcSwBlockCRC(&block, crc);
  }
}
/****************************************************************************************/
#ifndef LOADER
void Cstorage::eraseSw() {
  u16 page;
  u08 dat[pageSize];
  memset(dat, 0xFF, pageSize);
  for (page = swBasePage; page <= swTopPage; page++) {
    flashdrv->write(page, dat);
  }
}
/****************************************************************************************/
void Cstorage::setNetConf() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    eeprom_read_block(&eeprom, 0, sizeof(eeprom));
    eeprom.networkConfigured = true;
    eeprom_write_block(&eeprom, 0, sizeof(eeprom));
  }
}
/****************************************************************************************/
void Cstorage::setConfig(sConfig _config) {

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (_config.autoConnectTime.hour != 0) {
      eeprom_read_block(&eeprom, 0, sizeof(eeprom));
      eeprom.activeConfig.sw = _config.sw;
      eeprom.activeConfig.autoConnectTime = _config.autoConnectTime;
      eeprom.activeConfig.rssiReject = _config.rssiReject;
      eeprom_write_block(&eeprom, 0, sizeof(eeprom));
    }
  }
}
/****************************************************************************************/
void Cstorage::setMacAdr(sMacADR adr) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    eeprom_read_block(&eeprom, 0, sizeof(eeprom));
    eeprom.activeConfig.macAdr = adr;
    eeprom_write_block(&eeprom, 0, sizeof(eeprom));
  }
}
/****************************************************************************************/
void Cstorage::setNetConfig(sNetConfig netConfig) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    eeprom_read_block(&eeprom, 0, sizeof(eeprom));
    eeprom.activeConfig.netConfig = netConfig;
    eeprom_write_block(&eeprom, 0, sizeof(eeprom));
  }
}
/****************************************************************************************/
void Cstorage::setServerConfig(sServerConfig serverConfig) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    eeprom_read_block(&eeprom, 0, sizeof(eeprom));
    eeprom.activeConfig.serverConfig = serverConfig;
    eeprom_write_block(&eeprom, 0, sizeof(eeprom));
  }
}
/****************************************************************************************/
void Cstorage::setReaderID(u32 _readerID) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    eeprom_read_block(&eeprom, 0, sizeof(eeprom));
    eeprom.activeConfig.readerID = _readerID;
    eeprom.readerConfigured = true;
    eeprom_write_block(&eeprom, 0, sizeof(eeprom));
  }
}
/****************************************************************************************/
void Cstorage::setSwBlock(sSwBlock* block) {
  u08 buf[pageSize];
  u08 ofst;
  u16 page;
  page = block->meta.blockNumber / swBlocksPerPage + swBasePage;
  ofst = (block->meta.blockNumber % swBlocksPerPage) * sizeof(sSwBlock);
  flashdrv->read(buf, page);
  memcpy(&buf[ofst], block, sizeof(sSwBlock));
  flashdrv->write(page, buf);
}
#endif
/****************************************************************************************/
void Cstorage::calcSwBlockCRC(sSwBlock* block, u16* crc) {
  getSwBlock(block);
  *crc = crc16((u08*) &block->dat, block->meta.blockSize, *crc);
}
/****************************************************************************************/
void Cstorage::getSwBlock(sSwBlock* block) {
  u08 buf[pageSize];
  u08 ofst;
  u16 page;
  page = block->meta.blockNumber / swBlocksPerPage + swBasePage;
  ofst = (block->meta.blockNumber % swBlocksPerPage) * sizeof(sSwBlock);
  flashdrv->read(buf, page);
  memcpy(block, &buf[ofst], sizeof(sSwBlock));
}
/****************************************************************************************/
#ifndef LOADER
void Cstorage::eraseFlash() {
  flash.magic = FLASH_MAGIC_NUM_0;
  flash.currentSw.revision = 0;
  flash.currentSw.size = 0;
  flash.currentSw.crc = 0;
  flash.currentSw.numBlocks = 0;
  flash.storedSw.revision = 0;
  flash.storedSw.size = 0;
  flash.storedSw.crc = 0;
  flash.storedSw.numBlocks = 0;
  // Save
  flashdrv->write(configBasePage, (u08*) &flash);
}
#endif
/****************************************************************************************/
#ifndef LOADER
void Cstorage::eraseEeprom() {
  eeprom.magic = EEPROM_MAGIC_NUM_1;
  eeprom.networkConfigured = false;
  eeprom.readerConfigured = false;
  eeprom.activeConfig.autoConnectTime = defAutoTime;
  eeprom.activeConfig.readerID = 0x0;
  eeprom.activeConfig.rf.adr = CC1101_DEFVAL_ADDR;
  eeprom.activeConfig.rf.channel = CC1101_DEFVAL_CHANNR;
  eeprom.activeConfig.sw.crc = 0x00000000;
  eeprom.activeConfig.sw.revision = 0x0;
  eeprom.activeConfig.sw.numBlocks = 0;
  eeprom.activeConfig.sw.blockSize = 0;
  eeprom.activeConfig.sw.size = 0;
  eeprom.activeConfig.macAdr = defMacAdr;
  eeprom.activeConfig.serverConfig.ip = defServerIP[0];
  eeprom.activeConfig.serverConfig.port = defServerPort;
  eeprom.activeConfig.netConfig.src = defReaderIP[0];
  eeprom.activeConfig.netConfig.netMask = defNetMask[0];
  eeprom.activeConfig.netConfig.useDHCP = defDHCP[0];
  eeprom.activeConfig.netConfig.dns = defDns[0];
  eeprom.activeConfig.netConfig.gateway = defGateway[0];
  eeprom.activeConfig.rssiReject = defRssiThreshold;

  // Save the default config
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    eeprom_write_block(&eeprom, 0, sizeof(eeprom));
  }
}
#endif
