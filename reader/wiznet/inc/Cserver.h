#ifndef CSERVER_H
#define CSERVER_H
/****************************************************************************************/
#include "network.h"
#include "MCP7940x.h"
#include "iopins.h"
#include "payload.h"
#include "storage.h"
/****************************************************************************************/
#define LOG_READ_SIZE 128
/****************************************************************************************/
using namespace Cnetwork;
using namespace XREADER;
/****************************************************************************************/
typedef enum {
  TASK_IDLE, TASK_DONE, TASK_BUSY, TASK_FAIL, TASK_NACK
} eTask;
/****************************************************************************************/
class Cserver {
    CNetwork* Network;
    Cstorage* store;
    eTask task;
    u08 transactNum;
    bool sendCommand(eOpcodes Opcode);
    bool sendCommand(sCmd cmd);
  public:
    Cserver(Cnetwork::CNetwork* Network, Cstorage* _store);
    void service(void);
    bool setStatus(sStatus status);
    bool setEvent(sEvent02 event);
    bool getDate(sDate* date);
    bool getConfig(sConfig* config);
    bool getSwBlock(sSwBlock* block);
    bool getReaderID(u32* _readerID);
    bool getMacADR(sMacADR* _macAdr);
    bool getServerConfig(sServerConfig* _dstServer);
    bool getNetConfig(sNetConfig* _netConfig);
};
/****************************************************************************************/
#endif
