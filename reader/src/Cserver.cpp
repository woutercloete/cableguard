/****************************************************************************************/
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <string.h>
/****************************************************************************************/
extern "C" {
#include <util/delay.h>
}
#include "avrlibtypes.h"
#include "Cserver.h"
#include "network.h"
#include "payload.h"
#include "crc.h"
/****************************************************************************************/
using namespace XREADER;
/****************************************************************************************/
#define SERVER_TIMEOUT 2000
#define SERVER_RETRIES 3
/****************************************************************************************/
Cserver::Cserver(Cnetwork::CNetwork* Network, Cstorage* _store) {
  this->Network = Network;
  this->store = _store;
  transactNum = 0;
}
/****************************************************************************************/
void Cserver::service(void) {
  Network->service();
  if (Network->packetAvailable()) {
    sResp* response = (sResp*) Network->payload;
    task = TASK_NACK;
    if (response->ack) {
      task = TASK_DONE;
    }
    Network->reset();
  }
}
/****************************************************************************************/
bool Cserver::setStatus(sStatus status) {
  sCmd cmd;
  cmd.Opcode = SET_STATUS;
  memcpy(&cmd.status, &status, sizeof(sStatus));
  return sendCommand(cmd);
}
/****************************************************************************************/
bool Cserver::setEvent(sEvent02 event) {
  sCmd cmd;
  cmd.Opcode = SET_EVENT_02;
  memcpy(&cmd.event, &event, sizeof(sEvent02));
  return sendCommand(cmd);
}
/****************************************************************************************/
bool Cserver::getDate(sDate* date) {
  if (sendCommand(GET_DATE)) {
    sResp* response = (sResp*) Network->payload;
    if (response->Opcode == GET_DATE && response->ack == true) {
      *date = response->date;
      return true;
    } else
      return false;
  }
  return false;
}
/****************************************************************************************/
bool Cserver::getConfig(sConfig* config) {
  if (sendCommand(GET_CONFIG)) {
    sResp* response = (sResp*) Network->payload;
    if (response->Opcode == GET_CONFIG && response->ack == true) {
      *config = response->config;
      return true;
    } else
      return false;
  }
  return false;
}
/****************************************************************************************/
bool Cserver::getNetConfig(sNetConfig* _netConfig) {
  if (sendCommand(GET_NET_CONFIG)) {
    sResp* response = (sResp*) Network->payload;
    if (response->Opcode == GET_NET_CONFIG && response->ack == true) {
      *_netConfig = response->netConfig;
      return true;
    } else
      return false;
  }
  return false;
}
/****************************************************************************************/
bool Cserver::getServerConfig(sServerConfig* _dstServer) {
  if (sendCommand(GET_DST_CONFIG)) {
    sResp* response = (sResp*) Network->payload;
    if (response->Opcode == GET_DST_CONFIG && response->ack == true) {
      *_dstServer = response->server;
      return true;
    } else
      return false;
  }
  return false;
}
/****************************************************************************************/
bool Cserver::getMacADR(sMacADR* _macAdr) {
  if (sendCommand(GET_MAC_ADR)) {
    sResp* response = (sResp*) Network->payload;
    if (response->Opcode == GET_MAC_ADR && response->ack == true) {
      *_macAdr = response->mac;
      return true;
    } else
      return false;
  }
  return false;
}
/****************************************************************************************/
bool Cserver::getReaderID(u32* _readerID) {
  if (sendCommand(GET_READER_ID)) {
    sResp* response = (sResp*) Network->payload;
    if (response->Opcode == GET_READER_ID && response->ack == true) {
      *_readerID = response->readerID;
      return true;
    } else
      return false;
  }
  return false;
}
/****************************************************************************************/
bool Cserver::getSwBlock(sSwBlock* block) {
  sCmd cmd;
  u08 retry = 0;
  cmd.Opcode = GET_SW_BLOCK;
  cmd.readerID = store->eeprom.activeConfig.readerID;
  cmd.meta = block->meta;
  while (retry < 4) {
    if (sendCommand(cmd)) {
      sResp* response = (sResp*) Network->payload;
      if (response->meta.blockNumber == cmd.meta.blockNumber
          && response->meta.blockRevision == cmd.meta.blockRevision) {
        block->meta = response->meta;
        memcpy(block->dat, response->dat, SW_BLOCK_SIZE);
        if (crc16((u08*) &block->dat, SW_BLOCK_SIZE, 0) == block->meta.blockCRC)
          return true;
      }
      retry++;
    }
  }
  return false;
}
/****************************************************************************************/
bool Cserver::sendCommand(eOpcodes Opcode) {
  sCmd cmd;
  cmd.Opcode = Opcode;
  cmd.readerID = store->getReaderID();
  return sendCommand(cmd);
}
/****************************************************************************************/
bool Cserver::sendCommand(sCmd cmd) {
  u32 timeout;
  u08 retry = 0;
  task = TASK_BUSY;
  while (retry < SERVER_RETRIES) {
    timeout = 0;
    Network->tx(transactNum++, 0, (u08*) &cmd, sizeof(sCmd));
    while (timeout < SERVER_TIMEOUT && task != TASK_DONE && task != TASK_NACK) {
      service();
      timeout++;
      _delay_ms(1);
    }
    if (task == TASK_DONE) {
      return true;
    }
    retry++;
  }
  return false;
}
/****************************************************************************************/
