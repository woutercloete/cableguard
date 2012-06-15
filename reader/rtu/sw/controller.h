/****************************************************************************************/
#ifndef CCONTROLLER_H
#define CCONTROLLER_H
/****************************************************************************************/
#include <avr/eeprom.h>
#include "main.h"
#include "sim300.h"
#include "tagtable.h"
#include "network.h"
/****************************************************************************************/
extern EEMEM u08 eePhoneNumbers[MAX_SMS_PHONENUMBERS][12];
extern EEMEM u08 eeSiteID;
extern EEMEM u08 eeIP[32];
extern EEMEM u08 EEsmsEnable;
extern Cuart DbgUart;
/****************************************************************************************/
namespace CONTROLLER {
  const u08 NUM_OF_SIGNALS = 2;
  const u08 TAGTABLE_SIGNAL = 0;
  const u08 MODEM_SIGNAL = 1;
  const u08 MAX_SMS_PHONENUMBERS = 2;
  typedef enum {
    ACCESS_DISABLED, ACCESS_AUTHORISED, ACCESS_GRANTED
  } eSiteState;
  class CController {
    private:
      Ctagtable* tagTable;
      CNetwork* network;
      u08 smsCnt;
      u08 Authorise;
      Csim300 *modem;
      u08 SiteID;
      u08 idValid;
      u08 ipValid;
      sSignal signal[NUM_OF_SIGNALS];
      c08 PhoneNumbers[MAX_SMS_PHONENUMBERS][12];
      c08 sim1PIN[6];
      c08 sim2PIN[6];
      u08 nrValid;
      u08 PIN1Invalid;
      void idle(void);
      void consoleService(void);
      void signalsService(void);
      void TagTableService(void);
    public:
      volatile u32 timer;
      CController(CRFID* _Rfid, Csim300 *_modem, CServer *_server, Ctagtable* _tagTable,
                  CNetwork* _network) {
        c08 str[32];
        c08 *pstr;
        tagTable = _tagTable;
        network = _network;
        Rfid = _Rfid;
        modem = _modem;
        server = _server;
        smsCnt = 0;
        signal[TAGTABLE_SIGNAL].period = 10000000;
        signal[TAGTABLE_SIGNAL].timer = 0;
        signal[TAGTABLE_SIGNAL].state = SIG_RESET;
        signal[MODEM_SIGNAL].period = 2500000;
        signal[MODEM_SIGNAL].timer = 0;
        signal[MODEM_SIGNAL].state = SIG_RESET;
        nrValid = false;
        ipValid = false;
        idValid = false;
        SiteID = eeprom_read_byte(&eeSiteID);
        if (SiteID > 0 && SiteID < 0xFF) {
          idValid = true;
        }
        server->SetSiteNr(SiteID);

        eeprom_read_block(PhoneNumbers, eePhoneNumbers, sizeof(eePhoneNumbers));
        if (strcmp(PhoneNumbers[1], "\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0") == 0
            || strcmp(PhoneNumbers[1], "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF")
                == 0) {
          nrValid = false;
        } else {
          nrValid = true;
        }
        memset(str, 0, 32);
        eeprom_read_block(str, eeIP, sizeof(eeIP));
        str[31] = 0;
        pstr = strtok(str, ":");
        if (pstr) {
          ipValid = true;
        } else {
          ipValid = false;
        }
        strcpy(modem->serverIP, pstr);
        pstr = strtok(NULL, ":");
        if (pstr) {
          strcpy(modem->port, pstr);
        }
        pstr = strtok(NULL, ":");
        if (*pstr == '1') {
          modem->usedns = true;
        } else {
          modem->usedns = false;
        }
        modem->smstx_en = eeprom_read_byte(&EEsmsEnable);
      }
      void Service(void);
      void HandleRxData(void);
      void Setup(void);
      /****************************************************************************************/
      void checkTags() {
        sRadioPacket* pkt;
        sEvent event;
        c08 sms[64];
        // If a new packet came in add it to the tag list.
        if (network->packetAvailable()) {
          pkt = (sRadioPacket*) network->payload;
          if (pkt->crc_ok == 0x1) {
            tagTable->add(pkt);
          }
          network->reset();
        }
        // Post new events to server
        if (!tagTable->events.empty()) {
          tagTable->events.remove(&event, 1);
          if (event.eventType == TAG::OUT_RANGE
              || event.eventType == TAG::MOVEMENT_CHANGED) {
            sprintf(sms, "CABLE ALARM AT SITE  %d ! ", SiteID);
            DbgUart.sendStr("\n\r================\n\r");
            DbgUart.send(sms, strlen(sms));
            DbgUart.sendStr("\n\r================\n\r");
            if (modem->mdmState == MDM_READY) {
              modem->SendSMS(PhoneNumbers[0], sms);
            }
          }
        }
      }
  };
}
}
#endif

