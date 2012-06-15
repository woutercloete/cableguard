#include <util/atomic.h>
#include <util/delay.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "controller.h"
#include "uart.h"
#include "sim300.h"
#include "ds3231.h"

using namespace CCONTROLLER;

EEMEM u08 eePhoneNumbers[MAX_SMS_PHONENUMBERS][12] = { "0836524564", "0836524564" };
EEMEM u08 eeSiteID = 222;
EEMEM u08 eeIP[32] = { "196.214.135.44:3000:0" };
EEMEM u08 EEsmsEnable = 1;

extern Cuart DbgUart;
extern Crtc rtc;
/****************************************************************************************/
void menu(void) {
  DbgUart.sendStr_P(PSTR("\n\n\r\t\tMENU"));
  DbgUart.sendStr_P((PSTR("\n\rt\t: Display Tags Found")));
  DbgUart.sendStr_P((PSTR("\n\rq\t: GetSignalQuality")));
  DbgUart.sendStr_P((PSTR("\n\ri\t: Get the IP status")));
  DbgUart.sendStr_P((PSTR("\n\rs\t: Send Test SMS")));
  DbgUart.sendStr_P((PSTR("\n\rc\t: Check Simcard")));
  DbgUart.sendStr_P((PSTR("\n\r$\t: Check Simcard Balance")));
  DbgUart.sendStr_P((PSTR("\n\rd\t: Send Test Data to Server tag=123")));
  DbgUart.sendStr_P((PSTR("\n\rg\t: Get SMS PhoneNumbers")));
  DbgUart.sendStr_P((PSTR("\n\rn\t: Set SMS PhoneNumber")));
  DbgUart.sendStr_P((PSTR("\n\rp\t: Set PIN for SIM 1")));
  DbgUart.sendStr_P((PSTR("\n\rx\t: Restart Modem")));
  DbgUart.sendStr_P((PSTR("\n\rz\t: Restart Modem TCP IP Stack")));
  DbgUart.sendStr_P((PSTR("\n\rw\t: Get Site ID")));
  DbgUart.sendStr_P((PSTR("\n\rW\t: Set Site ID")));
  DbgUart.sendStr_P((PSTR("\n\ra\t: Get Server IP Address")));
  DbgUart.sendStr_P((PSTR("\n\rA\t: Set Server IP Address")));
  DbgUart.sendStr_P((PSTR("\n\rT\t: Get Time")));
  DbgUart.sendStr_P((PSTR("\n\rv\t: Get Modem Time")));

}
/****************************************************************************************/
void CController::Setup(void) {
  u08 cmd;
  c08 *pstr;
  c08 str[32];
  u08 i, j;
  u16 year = 0;
  u08 month = 0;
  u08 day = 0;
  u08 wday = 0;
  u08 hour = 0;
  u08 min = 0;
  u08 state;
  cmd = 0;

  DbgUart.sendStr_P(PSTR("\n\r === First Set Phone Number and Site ID === \n\r"));
  DbgUart.sendStr_P((PSTR("\n\rg\t: Get SMS PhoneNumbers")));
  DbgUart.sendStr_P((PSTR("\n\rn\t: Set SMS PhoneNumber")));
  DbgUart.sendStr_P((PSTR("\n\rw\t: Get Site ID")));
  DbgUart.sendStr_P((PSTR("\n\rW\t: Set Site ID")));
  DbgUart.sendStr_P((PSTR("\n\ra\t: Get Server IP Address")));
  DbgUart.sendStr_P((PSTR("\n\rA\t: Set Server IP Address")));
  DbgUart.sendStr_P((PSTR("\n\rt\t: Get Time")));
  DbgUart.sendStr_P((PSTR("\n\rT\t: Set Time & Date")));

  cmd = 0;
  i = 0;
  DbgUart.sendStr_P((PSTR("\n\r\t Press key to setup Device - x to exit")));
  DbgUart.clearRx();
  while (1) {
    DbgUart.receive(&cmd, 1);
    _delay_ms(1000);
    i++;
    if (i > 5 || cmd > 0x30 ) {
      if (cmd == 'x') {
        cmd = 0;
      }
      break;
    };
  }

  if(i>=5){
    return;
  }

  while (1) {
    cmd = 0;
    DbgUart.receive(&cmd, 1);
    switch (cmd) {
      case ' ':
        DbgUart.sendStr_P((PSTR("\n\rg\t: Get SMS PhoneNumbers")));
        DbgUart.sendStr_P((PSTR("\n\rn\t: Set SMS PhoneNumber")));
        DbgUart.sendStr_P((PSTR("\n\rw\t: Get Site ID")));
        DbgUart.sendStr_P((PSTR("\n\rW\t: Set Site ID")));
        DbgUart.sendStr_P((PSTR("\n\ra\t: Get Server IP Address")));
        DbgUart.sendStr_P((PSTR("\n\rA\t: Set Server IP Address")));
        DbgUart.sendStr_P((PSTR("\n\rt\t: Get Time")));
        DbgUart.sendStr_P((PSTR("\n\rT\t: Set Time & Date")));
        break;
      case 'x':
        return;
        break;
      case 't':
        DbgUart.sendStr_P((PSTR("\n\r")));
        DbgUart.sendStr(rtc.getTimestamp());
        break;
      case 'T':
        DbgUart.sendStr_P((PSTR("\n\rSet Time & Date")));
        j = 0;
        state = 0;
        DbgUart.sendStr_P((PSTR("\n\rSet Year")));
        while (state < 6) {
          if (DbgUart.receive(&cmd, 1)) {
            DbgUart.uprintf("%c", cmd);
            if (cmd == '\r' || cmd == '\n') {
              state++;

              switch (state) {
                case 1:
                  str[j] = 0;
                  j = 0;
                  year = atoi(str);
                  DbgUart.sendStr_P((PSTR("\n\rSet Month")));
                  break;
                case 2:
                  str[j] = 0;
                  j = 0;
                  month = atoi(str);
                  DbgUart.sendStr_P((PSTR("\n\rSet Date")));
                  break;
                case 3:
                  str[j] = 0;
                  j = 0;
                  day = atoi(str);
                  DbgUart.sendStr_P((PSTR("\n\rSet Day [1-7]")));
                  break;
                case 4:
                  str[j] = 0;
                  j = 0;
                  wday = atoi(str);
                  DbgUart.sendStr_P((PSTR("\n\rSet Hour")));
                  break;
                case 5:
                  str[j] = 0;
                  j = 0;
                  hour = atoi(str);
                  DbgUart.sendStr_P((PSTR("\n\rSet Minutes")));
                  break;
                case 6:
                  str[j] = 0;
                  j = 0;
                  min = atoi(str);
                  DbgUart.sendStr_P((PSTR("\n\rFinished")));
                  rtc.setDate(year, month, day, wday, hour, min);
                  break;
              }
            } else {
              str[j++] = cmd;
            }
          }
        }

        break;

      case 'a':
        DbgUart.sendStr_P((PSTR("\n\rServer IP Address : ")));
        DbgUart.sendStr(modem->serverIP);
        DbgUart.sendStr(":");
        DbgUart.sendStr(modem->port);
        break;
      case 'A':
        DbgUart.sendStr_P((PSTR("\n\rSet Server IP Address : xxx.yyy.zzz:port:dns\n\r")));
        j = 0;
        while (1) {
          if (DbgUart.receive(&cmd, 1)) {
            DbgUart.uprintf("%c", cmd);
            if (j >= 24) {
              DbgUart.uprintf("\n\rInvalid format");
              break;
            }
            if (cmd == '\r' || cmd == '\n') {
              str[j] = ':';
              str[j + 1] = 0;
              ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                eeprom_write_block(str, eeIP, sizeof(eeIP));
              }
              pstr = strtok(str, ":");
              if (pstr) {
                ipValid = true;
              } else {
                ipValid = false;
              }
              strcpy(modem->serverIP, pstr);
              pstr = strtok(NULL, ":");
              strcpy(modem->port, pstr);
              pstr = strtok(NULL, ":");
              if (*pstr == '1') {
                modem->usedns = true;
              } else {
                modem->usedns = false;
              }
              DbgUart.uprintf("\n\rServer IP is set OK");
              ipValid = true;
              break;
            } else {
              str[j++] = cmd;
            }
          }
        }
        break;
      case 'w':
        DbgUart.uprintf("\n\rSITE ID : %d", SiteID);
        break;
      case 'W':
        DbgUart.uprintf("\n\r0 < SET SITE ID < 255\n\r");
        j = 0;
        while (1) {
          if (DbgUart.receive(&cmd, 1)) {
            DbgUart.uprintf("%c", cmd);
            if (cmd == '\r' || j >= 3 || cmd == '\n') {
              str[j] = 0;
              i = atoi(str);
              ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                eeprom_write_byte(&eeSiteID, i);
              }
              SiteID = i;
              server->SetSiteNr(i);
              DbgUart.uprintf("\n\rSiteID is set OK");
              idValid = true;
              break;
            } else {
              str[j++] = cmd;
            }
          }
        }
        break;
      case 'n':
      case 'N':
        j = 0;
        i = 0;
        DbgUart.uprintf("\n\rSet %d SMS Phone Numbers\n\r", MAX_SMS_PHONENUMBERS);
        while (1) {
          if (DbgUart.receive(&cmd, 1)) {
            DbgUart.uprintf("%c", cmd);
            if (cmd == '\r' || j >= 10 || cmd == '\n') {
              str[j] = 0;
              strcpy(PhoneNumbers[i], str);
              DbgUart.uprintf("\n\rNumber[%d]=%s Set %d of %d\n\r", i, str, i + 1,
                              MAX_SMS_PHONENUMBERS);
              ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                eeprom_write_block(PhoneNumbers[i], eePhoneNumbers[i],
                                   sizeof(eePhoneNumbers[0]));
              }
              j = 0;
              i++;
              if (i == MAX_SMS_PHONENUMBERS) {
                nrValid = true;
                break;
              }
            } else {
              str[j++] = cmd;
            }
          }
        }
        cmd = 0;
        break;
      case 'g':
        for (i = 0; i < MAX_SMS_PHONENUMBERS; i++) {
          DbgUart.uprintf("\n\rPhoneNumber[%d]:\t%s", i, PhoneNumbers[i]);
        }
        break;
      default:
        break;
    }
  }
}

void CController::consoleService(void) {
  u08 cmd;
  c08 tmp;
  c08 *pstr;
  c08 str[32];
  u08 i, j;
  cmd = 0;

  //TagTableService();
  if (signal[MODEM_SIGNAL].state == SIG_TRIGGERED) {
    signal[MODEM_SIGNAL].state = SIG_RESET;
    //modem->UpdateMdmStatus();
  }

  DbgUart.receive(&cmd, 1);
  switch (cmd) {
    case 's':
      if (nrValid) {
        modem->SendSMS("0836524564", "GPRS CORE RFID SITE MANAGER DEMO");
      } else {
        DbgUart.sendStr("\n\rSet Phone Number: ");
      }
      break;
    case 'q':
      modem->GetSignalQuality();
      break;

    case 'c':
      modem->SIMCheckReady();
      break;

    case '1':
      server->SetTagInRange(22222);
      break;

    case '2':
      server->SetTagInRange(12345);
      break;
    case '3':
      server->SetTagDuress(33333);
      break;
    case '4':
      server->SetTagOutRange(44444);
      break;

    case 'i':
      modem->setNextState(MDM_IP_STATUS);
      break;

    case 'z':
      modem->setNextState(MDM_START_IP);
      break;

    case 'r':
      modem->setNextState(MDM_IPSHUTDOWN);
      break;

    case '$':
      modem->setNextState(MDM_GET_BALANCE);
      break;

    case 'g':
      for (i = 0; i < MAX_SMS_PHONENUMBERS; i++) {
        DbgUart.uprintf("\n\rPhoneNumber[%d]:\t%s", i, PhoneNumbers[i]);
      }
      break;

    case 'w':
      DbgUart.uprintf("\n\rSITE ID : %d", SiteID);
      break;
    case 'W':
      DbgUart.uprintf("\n\r0 < SET SITE ID < 255\n\r");
      j = 0;
      while (1) {
        if (DbgUart.receive(&cmd, 1)) {
          DbgUart.uprintf("%c", cmd);
          if (cmd == '\r' || j >= 3 || cmd == '\n') {
            str[j] = 0;
            i = atoi(str);
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
              eeprom_write_byte(&eeSiteID, i);
            }
            SiteID = i;
            DbgUart.uprintf("\n\rSiteID is set OK");
            idValid = true;
            break;
          } else {
            str[j++] = cmd;
          }
        }
      }
      break;
    case 'T':
      DbgUart.sendStr_P((PSTR("\n\r")));
      DbgUart.sendStr(rtc.getTimestamp());
      break;
    case 'a':
      DbgUart.sendStr_P((PSTR("\n\rServer IP Address : ")));
      DbgUart.sendStr(modem->serverIP);
      DbgUart.sendStr(":");
      DbgUart.sendStr(modem->port);
      break;
    case 'A':
      DbgUart.sendStr_P((PSTR("\n\rSet Server IP Address : xxx.yyy.zzz:port:dns\n\r")));
      j = 0;
      while (1) {
        if (DbgUart.receive(&cmd, 1)) {
          DbgUart.uprintf("%c", cmd);
          if (j >= 24) {
            DbgUart.uprintf("\n\rInvalid format");
            break;
          }
          if (cmd == '\r' || cmd == '\n') {
            str[j] = ':';
            str[j + 1] = 0;
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
              eeprom_write_block(str, eeIP, sizeof(eeIP));
            }

            pstr = strtok(str, ":");
            if (pstr) {
              ipValid = true;
            } else {
              ipValid = false;
            }
            strcpy(modem->serverIP, pstr);
            pstr = strtok(NULL, ":");
            strcpy(modem->port, pstr);
            pstr = strtok(NULL, ":");
            if (*pstr == '1') {
              modem->usedns = true;
            } else {
              modem->usedns = false;
            }
            eeprom_write_block(str, eeIP, sizeof(eeIP));
            DbgUart.uprintf("\n\rServer IP is set OK");
            ipValid = true;
            break;
          } else {
            str[j++] = cmd;
          }
        }
      }
      break;
    case 'x':
      DbgUart.sendStr_P(PSTR("\r\nRESTART MODEM"));
      modem->mdmState = MDM_STARTUP;
      modem->cmdState = START;
      modem->initState = 1;

      break;

    case 'v':
      DbgUart.sendStr_P((PSTR("\n\rCOMMAND ")));
      while (1) {
        i = 0;
        do {
          cmd = 0;
          if (DbgUart.receive(&cmd, 1)) {
            DbgUart.send((c08 *) &cmd, 1);
            str[i++] = cmd;
          }
        } while (cmd != 0x0D);
        str[i++] = 0x00;

        if (str[0] == 'X' || str[0] == 'x') {
          DbgUart.sendStr_P((PSTR("\n\rEXIT\n\r")));
          break;
        }

        modem->pUart->sendStr(str);
        while (modem->pUart->receive((u08*) &tmp, 1)) {
          DbgUart.uprintf("%c", tmp);
        }
      }
      break;

    case 'n':
    case 'N':
      j = 0;
      i = 0;
      DbgUart.uprintf("\n\rSet %d SMS Phone Numbers\n\r", MAX_SMS_PHONENUMBERS);
      while (1) {
        if (DbgUart.receive(&cmd, 1)) {
          DbgUart.uprintf("%c", cmd);
          if (cmd == '\r' || j >= 10 || cmd == '\n') {
            str[j] = 0;
            strcpy(PhoneNumbers[i], str);
            DbgUart.uprintf("\n\rNumber[%d]=%s Set %d of %d\n\r", i, str, i + 1,
                            MAX_SMS_PHONENUMBERS);
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
              eeprom_write_block(PhoneNumbers[i], eePhoneNumbers[i],
                                 sizeof(eePhoneNumbers[0]));
            }
            j = 0;
            i++;
            if (i == MAX_SMS_PHONENUMBERS) {
              nrValid = true;
              break;
            }
          } else {
            str[j++] = cmd;
          }
        }
      }
      cmd = 0;
      break;

    case 'p':
      j = 0;
      DbgUart.sendStr_P(PSTR("\n\rSet SIM1 PIN\n\r"));
      while (1) {
        if (DbgUart.receive(&cmd, 1)) {
          DbgUart.uprintf("%c", cmd);
          if (cmd == '\r' || j >= 10 || cmd == '\n') {
            str[j] = 0;
            strcpy(sim1PIN, str);
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
              //eeprom_write_block(sim1PIN, eePIN1, sizeof(eePIN1));
            }
            PIN1Invalid = false;
            DbgUart.sendStr_P(PSTR("\n\rOK"));
            break;
          } else {
            str[j++] = cmd;
          }
        }
      }
      cmd = 0;
      break;

    case ' ':
    case 'm':
      menu();
      cmd = 0;
      break;

    case 't':
      DbgUart.sendStr_P((PSTR("\n\r=====TAGS Seen :=====\n\r")));
      for (i = 0; i < 8; i++) {
        if (Rfid->pTable[i].persistCnt >= 1) {
          DbgUart.uprintf("\t\r\n[ID:=%d cnt:%d rssi:=%d]", (u16) Rfid->pTable[i].tagID,
                          (u16) Rfid->pTable[i].persistCnt, (u16) Rfid->pTable[i].rssi);
        }
      }
      cmd = 0;
      break;
  }
}

void CController::HandleRxData(void) {
  c08 *pstr;
  if (modem->smsrx == true) {
    modem->smsrx = false;
    if (strstr_P(modem->sms.message, PSTR("SMS(E)SMS"))) {
      modem->smstx_en = true;
      eeprom_write_byte(&EEsmsEnable, true);
      DbgUart.sendStr_P(PSTR("\r\n ENABLE SMS\n\r"));
    } else if (strstr_P(modem->sms.message, PSTR("SMS(D)SMS"))) {
      DbgUart.sendStr_P(PSTR("\r\n DISABLE SMS\n\r"));
      modem->smstx_en = false;
      eeprom_write_byte(&EEsmsEnable, false);
    }
  }

  if (modem->gprsrx == true) {

    pstr = strstr_P(modem->gprsraw, PSTR("SMS(E)SMS"));
    if (pstr) {
      modem->smstx_en = true;
      eeprom_write_byte(&EEsmsEnable, true);
      DbgUart.sendStr_P(PSTR("\r\n ===> ENABLE SMS\n\r"));
    }
    pstr = strstr_P(modem->gprsraw, PSTR("SMS(D)SMS"));
    if (pstr) {
      DbgUart.sendStr_P(PSTR("\r\n ====>DISABLE SMS\n\r"));
      modem->smstx_en = false;
      eeprom_write_byte(&EEsmsEnable, false);
    }
    pstr = strstr_P(modem->gprsraw, PSTR("NUM1("));
    if (pstr) {
      strncpy(PhoneNumbers[0], pstr + strlen("NUM1("), 10);
      DbgUart.sendStr(PhoneNumbers[0]);
      eeprom_write_block(PhoneNumbers[0], eePhoneNumbers[0], sizeof(eePhoneNumbers[0]));
      DbgUart.sendStr_P(PSTR("\r\n ====>PROGRAM PHONE NUMBER1 OK \n\r"));
    }
    pstr = strstr_P(modem->gprsraw, PSTR("NUM2("));
    if (pstr) {
      strncpy(PhoneNumbers[1], pstr + strlen("NUM2("), 10);
      DbgUart.sendStr(PhoneNumbers[1]);
      eeprom_write_block(PhoneNumbers[1], eePhoneNumbers[1], sizeof(eePhoneNumbers[0]));
      DbgUart.sendStr_P(PSTR("\r\n ====>PROGRAM PHONE NUMBER 2 OK\n\r"));
    }
    memset(modem->gprsraw, 0, sizeof(modem->gprsraw));
    modem->gprsrx = false;
  }
}
/****************************************************************************************/
void CController::Service(void) {
  signalsService();
  consoleService();
  modem->Service();
  //HandleRxData();
  tagTable->service();
  if(modem->mdmState == MDM_READY){
    checkTags();
  }
  else {
    tagTable->events.clear();
  }
}
/****************************************************************************************/
void CController::TagTableService(void) {
  u08 i;
  c08 SmsTxt[64];
  u08 len;
  u08 ret;

  if (signal[TAGTABLE_SIGNAL].state == SIG_TRIGGERED) {
    signal[TAGTABLE_SIGNAL].state = SIG_RESET;
    for (i = 0; i < Rfid->tableLen; i++) {
      /****************************************************************/
      if (Rfid->pTable[i].status == TAG_PRESENT) {
        if (Rfid->pTable[i].smslogged == false) {
          sprintf(SmsTxt, "CULLINAN SITE NR %d -> TAG DETECTED! : ", SiteID);
          len = strlen(SmsTxt);
          ultoa(Rfid->pTable[i].tagID, &SmsTxt[len], 10);
          strcat(SmsTxt, " ");
          if (nrValid) {
            if (smsCnt == MAX_SMS_PHONENUMBERS) {
              if (Rfid->pTable[i].srvlogged == false) {
                if (server->SetTagInRange(Rfid->pTable[i].tagID)) {
                  DbgUart.sendStr_P((PSTR("\n\r !! New Tag Upload Issued !!")));
                  Rfid->pTable[i].srvlogged = true;
                  Rfid->pTable[i].smslogged = true;
                  smsCnt = 0;
                }
              }
            } else if (smsCnt < MAX_SMS_PHONENUMBERS) {
              if (modem->SendSMS(PhoneNumbers[smsCnt], SmsTxt)) {
                smsCnt++;
              }
            }
          }
        }
      }
      /****************************************************************/
      if (Rfid->pTable[i].duress) {
        sprintf(SmsTxt, "CULLINAN SITE NR %d -> TAG DURESS : ", SiteID);
        len = strlen(SmsTxt);
        ultoa(Rfid->pTable[i].tagID, &SmsTxt[len], 10);
        strcat(SmsTxt, " ");
        if (nrValid) {
          if (smsCnt < MAX_SMS_PHONENUMBERS) {
            ret = modem->SendSMS(PhoneNumbers[smsCnt], SmsTxt);
            if (ret) {
              // Previous SMS was send successfully
              smsCnt++;
            }
            if (smsCnt == MAX_SMS_PHONENUMBERS) {
              smsCnt = 0;
              Rfid->pTable[i].smslogged = true;
              Rfid->pTable[i].duress = 0;
            }
          }
        }
        if (server->SetTagDuress(Rfid->pTable[i].tagID)) {
          DbgUart.sendStr_P((PSTR("\n\r Duress Upload Issued")));
        }
      }
      /****************************************************************/
      if (Rfid->pTable[i].status == TAG_LOST) {
        if (Rfid->pTable[i].smslogged == false) {
          sprintf(SmsTxt, "CULLINAN SITE NR %d -> TAG LOST : ", SiteID);
          len = strlen(SmsTxt);
          ultoa(Rfid->pTable[i].tagID, &SmsTxt[len], 10);
          strcat(SmsTxt, " ");
          if (nrValid) {
            if (smsCnt < MAX_SMS_PHONENUMBERS) {
              if (modem->SendSMS(PhoneNumbers[smsCnt], SmsTxt)) {
                smsCnt++;
              }
            } else if (Rfid->pTable[i].srvlogged == false
                && smsCnt == MAX_SMS_PHONENUMBERS) {
              if (server->SetTagOutRange(Rfid->pTable[i].tagID)) {
                DbgUart.sendStr_P((PSTR("\n\rLost Tag Upload Issued")));
                smsCnt = 0;
                Rfid->pTable[i].smslogged = true;
                Rfid->pTable[i].srvlogged = true;
                memset((u08 *) &Rfid->pTable[i], 0, sizeof(sRfidTag));
              }
            }
          }
        }
      }
    }
  }
}

/****************************************************************************************/
void CController::signalsService(void) {
  u08 i;
  for (i = 0; i < NUM_OF_SIGNALS; i++) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      if (timer) {
        if (signal[i].timer < 0xFFFFFFFF && signal[i].state == SIG_PENDING) {
          signal[i].timer += timer;
          timer = 0;
        }
      }
    }
    if (signal[i].state == SIG_RESET) {
      signal[i].timer = 0;
      signal[i].state = SIG_PENDING;
    } else if (signal[i].state == SIG_STOP) {
      signal[i].timer = 0;
    } else if (signal[i].timer > signal[i].period) {
      signal[i].state = SIG_TRIGGERED;
    }
  }
}
/****************************************************************************************/
