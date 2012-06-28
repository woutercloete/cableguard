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
  }
  // Check if storage has been initilized.
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    eeprom_read_block(&eeprom, 0, sizeof(eeprom));
    if (eeprom.magic != EEPROM_MAGIC_NUM_0) {
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
      eeprom.config.sw = _config.sw;
      eeprom.config.autoConnectTime = _config.autoConnectTime;
      eeprom.config.rssiReject = _config.rssiReject;
      eeprom_write_block(&eeprom, 0, sizeof(eeprom));
    }
  }
}
/****************************************************************************************/
void Cstorage::setMacAdr(sMacADR adr) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    eeprom_read_block(&eeprom, 0, sizeof(eeprom));
    eeprom.config.macAdr = adr;
    eeprom_write_block(&eeprom, 0, sizeof(eeprom));
  }
}
/****************************************************************************************/
void Cstorage::setNetConfig(sNetConfig netConfig) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    eeprom_read_block(&eeprom, 0, sizeof(eeprom));
    eeprom.config.netConfig = netConfig;
    eeprom_write_block(&eeprom, 0, sizeof(eeprom));
  }
}
/****************************************************************************************/
void Cstorage::setServerConfig(sServerConfig serverConfig) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    eeprom_read_block(&eeprom, 0, sizeof(eeprom));
    eeprom.config.serverConfig = serverConfig;
    eeprom_write_block(&eeprom, 0, sizeof(eeprom));
  }
}
/****************************************************************************************/
void Cstorage::setReaderID(u32 _readerID) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    eeprom_read_block(&eeprom, 0, sizeof(eeprom));
    eeprom.config.readerID = _readerID;
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
  eeprom.magic = EEPROM_MAGIC_NUM_0;
  eeprom.networkConfigured = false;
  eeprom.readerConfigured = false;
  eeprom.config.autoConnectTime = defAutoTime;
  eeprom.config.readerID = 0x0;
  eeprom.config.rf.adr = CC1101_DEFVAL_ADDR;
  eeprom.config.rf.channel = CC1101_DEFVAL_CHANNR;
  eeprom.config.sw.crc = 0x00000000;
  eeprom.config.sw.revision = 0x0;
  eeprom.config.sw.numBlocks = 0;
  eeprom.config.sw.blockSize = 0;
  eeprom.config.sw.size = 0;
  eeprom.config.macAdr = defMacAdr;
  eeprom.config.serverConfig.ip = defServerIP;
  eeprom.config.serverConfig.port = defServerPort;
  eeprom.config.netConfig.src = defReaderIP;
  eeprom.config.netConfig.netMask = defNetMask;
  eeprom.config.netConfig.useDHCP = B_FALSE;
  eeprom.config.netConfig.dns = defDns;
  eeprom.config.netConfig.gateway = defGateway;

  // Save the default config
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    eeprom_write_block(&eeprom, 0, sizeof(eeprom));
  }
}
#endif
