#ifndef SIM300_H
#define SIM300_H
//*****************************************************************************
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "uart.h"
//*****************************************************************************
namespace SIM300 {
  const u08 RETRIES = 5;
  const u08 TIME_OUT = 10;
  const u08 MAX_TX_LEN = 255;
  const u08 MAX_RX_LEN = 128;
  typedef enum {
    UNKNOWN, READY
  } eState;
  /*****************************************************************************/
  typedef struct {
      u08 index;
      u08 name[20];
      u08 nameLen;
      u08 number[30];
      u08 numberLen;
      u08 addrType;
  } sPhoneBook;
  /*****************************************************************************/
  typedef struct {
      c08 nr[10];
      c08 phonenum[32];
      c08 timestamp[32];
      c08 message[140];
  } sSMS;
  //*****************************************************************************
  class Csim300 {
    private:
      Cuart *uart;
      Cpin* powerKey;
      Cpin* powerFET;
      u08 signalStrenght;
      char socket[8];
      c08 pincode[6];
      bool GetAtResp(PGM_P rspStr);
      void StartupTask(void);
      void ConfigureTask(void);
      bool Init(void);
      bool HandleAtCmd(c08 *str, PGM_P rspStr);
      bool PowerOff(void);
      bool PowerOn(void);
      void clearTimer(void);
    public:
      eState state;
      u08 simcard_ok;
      u08 signal_ok;
      u08 connect_ok;
      u08 registered_ok;
      /*******************************************************************************/
      Csim300(Cuart * _pUart, Cpin* _powerKey = 0, Cpin* _powerFET = 0) {
        uart = _pUart;
        powerKey = _powerKey;
        powerFET = _powerFET;
        if (powerKey)
          powerKey->enablePullup();
        state = UNKNOWN;
        signal_ok = false;
        simcard_ok = false;
        connect_ok = false;
        registered_ok = false;
        reset();
      }
      /*******************************************************************************/
      bool reset(void) {
        u08 wait = 0;
        if (!powerReset())
          return false;
        do {
          _delay_ms(100);
          if (wait++ > 20)
            return false;
        } while (uart->pinCTS->disabled());
        return true;
      }
      /*******************************************************************************/
      void service(void) {
//        strcpy_P(txcmd, PSTR("ATE0\r"));
//        if (HandleAtCmd(txcmd, AT_NONE)) {
//          mdmState = MDM_TEST_AT;
//        }
//        break;
//        case MDM_TEST_AT:
//        strcpy_P(txcmd, PSTR("AT\r"));
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          mdmState = MDM_ADD_IPHEAD;
//        }
//        break;
//        case MDM_GET_TIME:
//        strcpy_P(txcmd, PSTR("AT+CCLK\r"));
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          mdmState = MDM_READY;
//        }
//        break;
//        case MDM_ADD_IPHEAD:
//        strcpy_P(txcmd, PSTR("AT+CIPHEAD=1\r"));
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          mdmState = MDM_SET_SMS_TEXT_MODE;
//        }
//        break;
//        case MDM_SET_SMS_TEXT_MODE:
//        strcpy_P(txcmd, PSTR("AT+CMGF=1\r"));
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          mdmState = MDM_GET_SIGNAL_STRENGHT;
//          error_cnt = 0;
//        }
//        break;
//        case MDM_GET_SIGNAL_STRENGHT:
//        strcpy_P(txcmd, PSTR("AT+CSQ\r"));
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          pRx = rxmsg;
//          str = strsep(&pRx, ":");
//          //DbgUart.sendStr(str);
//          str = strsep(&pRx, ",");
//          //DbgUart.sendStr(str);
//          signalStrenght = atoi(str);
//          signal_ok = false;
//          if (signalStrenght == 99) {
//            error_cnt++;
//          } else if (signalStrenght > 5) {
//            signal_ok = true;
//            mdmState = MDM_GET_SIMCARD_STATUS;
//            error_cnt = 0;
//          } else {
//            error_cnt++;
//          }
//          if (error_cnt > 200) {
//            mdmState = MDM_FAILED;
//            DbgUart.sendStr_P(PSTR("\n\rSIGNAL STRENGHT TOO LOW!!"));
//          }
//        }
//        break;
//        case MDM_GET_SIMCARD_STATUS:
//        strcpy_P(txcmd, PSTR("AT+CPIN?\r"));
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          simcard_ok = false;
//          if (strstr_P(rxmsg, PSTR("+CPIN: READY"))) {
//            simcard_ok = true;
//            mdmState = MDM_IS_REGISTERED;
//            error_cnt = 0;
//          } else if (strstr_P(rxmsg, PSTR("+CPIN: SIM PIN"))) {
//            mdmState = MDM_SET_SIMCARD_PIN;
//            error_cnt = 0;
//          }
//        }
//        if (strstr_P(rxmsg, PSTR("+CME ERROR"))) {
//          mdmState = MDM_FAILED;
//          DbgUart.sendStr_P(PSTR("\n\rNO SIMCARD!!"));
//        }
//        break;
//        case MDM_SET_SIMCARD_PIN:
//        timeout = MDM_TIMEOUT_VAL * 5; //5seconds
//        strcpy_P(txcmd, PSTR("AT+CPIN="));
//        strcpy(pincode, "3470");
//        if (strlen(pincode) < 4) {
//          mdmState = MDM_FAILED; // TODO maybe goto a restart state?
//#if MDM_DEBUG_LEVEL > 2
//              DbgUart.sendStr_P(PSTR("\n\rNO VALID PIN SET"));
//#endif
//          break;
//        }
//        strcat(txcmd, pincode);
//        strcat_P(txcmd, PSTR("\r"));
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          simcard_ok = true;
//          mdmState = MDM_GET_SIMCARD_STATUS;
//        }
//        break;
//
//        case MDM_IS_REGISTERED:
//        strcpy_P(txcmd, PSTR("AT+CREG?\r"));
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          registered_ok = false;
//          if (strstr_P(rxmsg, PSTR("+CREG: 0,1"))) {
//            registered_ok = true;
//            mdmState = MDM_READY;
//          } else {
//            error_cnt++;
//            if (error_cnt > 200) {
//              mdmState = MDM_FAILED;
//              DbgUart.sendStr_P(PSTR("\n\rSIMCARD REGISTRATION FAILED!!"));
//            }
//          }
//        }
//        break;
//
//        case MDM_IP_STATUS:
//        strcpy_P(txcmd, PSTR("AT+CIPSTATUS\r"));
//        connect_ok = false;
//        if (HandleAtCmd(txcmd, AT_DATA)) {
//          if (strstr_P(rxmsg, AT_CONNECT)) {
//            mdmState = MDM_READY;
//            connect_ok = true;
//            DbgUart.sendStr("MODEM CONNECTED!!");
//          }
//          if (strstr_P(rxmsg, PSTR("INITIAL"))) {
//            mdmState = MDM_CONNECT_TO_SERVER;
//          } else if (strstr_P(rxmsg, PSTR("IP CLOSE"))) {
//            mdmState = MDM_START_IP;
//          } else if (strstr_P(rxmsg, PSTR("STATE: PDP DEACT"))) {
//            mdmState = MDM_IPSHUTDOWN;
//          } else {
//            mdmState = MDM_READY;
//          }
//        }
//        break;
//
//        /*******************************************************************************/
//        case MDM_READ_SMS:
//        strcpy_P(txcmd, PSTR("AT+CMGR="));
//        strcat(txcmd, sms.nr);
//        strcat_P(txcmd, PSTR("\r"));
//        if (HandleAtCmd(txcmd, AT_DATA)) {
//          strcpy(sms.message, rxmsg);
//          mdmState = MDM_READY;
//          smsrx = true;
//        }
//        break;
//
//        case MDM_SEND_SMS:
//        timeout = MDM_TIMEOUT_VAL * 5; //5seconds
//#ifdef SMS_DEBUG
//            mdmState = MDM_READY;
//            DbgUart.sendStr_P(PSTR("\n\r ## SMS SEND OK ##"));
//#else
//        strcpy_P(txcmd, PSTR("AT+CMGS=\""));
//        strcat(txcmd, sms.phonenum);
//        strcat_P(txcmd, PSTR("\"\r"));
//        if (HandleAtCmd(txcmd, AT_RDY)) {
//          pUart->sendStr(sms.message);
//          pUart->sendStr_P(PSTR("\x1A"));
//          mdmState = MDM_SMS_WAIT_SEND_OK;
//        } else if (cmdState == SEND) { //a timeout occured
//          pUart->sendStr_P(PSTR("\x1B")); // send an escape
//        }
//        break;
//
//        case MDM_SMS_WAIT_SEND_OK:
//        timeout = MDM_TIMEOUT_VAL * 5; //5seconds
//        if (GetAtResp(AT_OK)) {
//          DbgUart.sendStr_P(PSTR("\n\rSMS SEND OK"));
//          mdmState = MDM_READY;
//        } else if (cmdState == SEND) { //a timeout occured
//          DbgUart.sendStr_P(PSTR("\n\rSMS SEND ERROR"));
//          mdmState = MDM_READY;
//          //taskRetry++;
//        }
//#endif
//        break;
//
//        case MDM_CONNECT_TO_SERVER:
//        strcpy_P(txcmd, PSTR("AT+CGDCONT=1,\"IP\",\"internet\"\r"));
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          mdmState = MDM_SET_SHOW_REMOTE_IP;
//        }
//        break;
//        case MDM_SET_SHOW_REMOTE_IP:
//        strcpy_P(txcmd, PSTR("AT+CIPSRIP=0\r"));
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          mdmState = MDM_SET_DNS_CONNECT;
//        }
//        break;
//        case MDM_SET_DNS_CONNECT:
//        failState = MDM_IPSHUTDOWN;
//        if (usedns) {
//          strcpy_P(txcmd, PSTR("AT+CDNSORIP=1\r"));
//          if (HandleAtCmd(txcmd, AT_OK)) {
//            mdmState = MDM_SET_DNS_CONFIG;
//          }
//        } else {
//          strcpy_P(txcmd, PSTR("AT+CDNSORIP=0\r"));
//          if (HandleAtCmd(txcmd, AT_OK)) {
//            mdmState = MDM_SET_TCP_PORT;
//          }
//        }
//        break;
//        case MDM_SET_DNS_CONFIG:
//        failState = MDM_IPSHUTDOWN;
//        strcpy_P(txcmd, PSTR("AT+CDNSCFG=\""));
//        strcat(txcmd, DYNDNS_IP);
//        strcat_P(txcmd, PSTR("\"\r"));
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          mdmState = MDM_SET_TCP_PORT;
//        }
//        break;
//        case MDM_SET_TCP_PORT:
//        failState = MDM_IPSHUTDOWN;
//
//        strcpy_P(txcmd, PSTR("AT+CLPORT=\"TCP\",\""));
//        strcat(txcmd, port);
//        strcat(txcmd, "\"\r");
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          mdmState = MDM_START_TCP_TASK;
//        }
//        break;
//        case MDM_START_TCP_TASK:
//        failState = MDM_IPSHUTDOWN;
//        timeout = MDM_TIMEOUT_VAL * 30; //30 sekondes
//        strcpy_P(txcmd, PSTR("AT+CSTT=\"internet\",\"\",\"\"\r"));
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          mdmState = MDM_ACTIVATE_PDP;
//        }
//        break;
//        case MDM_ACTIVATE_PDP:
//        failState = MDM_IPSHUTDOWN;
//        timeout = MDM_TIMEOUT_VAL * 30; //30 sekondes
//        strcpy_P(txcmd, PSTR("AT+CIICR\r"));
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          mdmState = MDM_GET_IP;
//        }
//        break;
//        case MDM_GET_IP:
//        failState = MDM_IPSHUTDOWN;
//        timeout = MDM_TIMEOUT_VAL * 10; //10 sekondes
//        strcpy_P(txcmd, PSTR("AT+CIFSR\r"));
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          mdmState = MDM_START_IP;
//        }
//        if (cmdState == SEND) {
//          DbgUart.sendStr(rxmsg);
//          cmdState = START;
//          mdmState = MDM_START_IP;
//        }
//        break;
//        case MDM_START_IP:
//        timeout = MDM_TIMEOUT_VAL * 30; //20 sekondes
//        failState = MDM_IPSHUTDOWN;
//        strcpy(txcmd, "AT+CIPSTART=\"TCP\",\"");
//        strcat(txcmd, serverIP);
//        strcat(txcmd, "\",\"");
//        strcat(txcmd, port);
//        strcat(txcmd, "\"\r");
//        connect_ok = false;
//        if (HandleAtCmd(txcmd, AT_CONNECT)) {
//          connect_ok = true;
//          DbgUart.sendStr("MODEM CONNECTED!!");
//          mdmState = MDM_IP_STATUS;
//        }
//        if (cmdState == SEND) {
//          cmdState = START;
//          mdmState = MDM_IP_STATUS;
//        }
//        break;
//        case MDM_IP_SEND:
//        failState = MDM_START_IP;
//        if (HandleAtCmd("AT+CIPSEND\r", AT_RDY)) {
//          mdmState = MDM_IPSEND_VERIFY;
//          cmdState = START;
//          DbgUart.sendStr(txcmd);
//          pUart->sendStr(txcmd);
//          pUart->sendStr_P(PSTR("\x1A\r"));
//        }
//        break;
//        case MDM_IPSEND_VERIFY:
//        timeout = MDM_TIMEOUT_VAL * 30;
//        failState = MDM_IPSHUTDOWN;
//        if (GetAtResp(AT_SEND_OK)) {
//          DbgUart.sendStr("\n\rSUKSES");
//          mdmState = MDM_READY;
//        } else if (cmdState == SEND) { //a timeout occured
//#if MDM_DEBUG_LEVEL > 2
//            DbgUart.sendStr_P(PSTR("\n\rDATA SEND ERROR"));
//#endif
//          mdmState = MDM_IP_SEND;
//          //taskRetry++;
//        }
//        break;
//
//        case MDM_IPSHUTDOWN:
//        strcpy_P(txcmd, PSTR("AT+CIPSHUT\r"));
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          mdmState = MDM_CONNECT_TO_SERVER;
//        }
//        connect_ok = false;
//        break;
//
//        case MDM_GET_BALANCE:
//        timeout = MDM_TIMEOUT_VAL * 10; //10 sekondes
//        strcpy_P(txcmd, PSTR("ATD*100#\r"));
//        if (HandleAtCmd(txcmd, AT_DATA)) {
//          mdmState = MDM_READY;
//          DbgUart.sendStr(rxmsg);
//        }
//        break;
//
//        case MDM_FAILED:
//        mdmState = MDM_STARTUP;
//        cmdState = START;
//        break;
//
//        case MDM_READY:
//        break;
//
//        default:
//        break;
      }
      bool SIMCheckReady(void);
      void UpdateMdmStatus(void);
      bool SIMSetPin(c08* pin);
      bool GetSignalQuality(void);
      bool SendSMS(char *PhoneNumber, char *Message);
      /*******************************************************************************/
      bool sendCmd(c08 *str, PGM_P rspStr) {
        u08 retry = 0;
        u08 wait;
        c08 msg[MAX_RX_LEN];
        do {
          uart->sendStr(str);
          wait = 0;
          do {
            _delay_ms(100);
            if (wait++ > TIME_OUT)
              break;
          } while (uart->rxnum() < strlen(rspStr));
          uart->receive((u08*) msg, MAX_RX_LEN);
          if (retry++ > RETRIES)
            return false;
        } while (!strstr_P(msg, (rspStr)));
        return true;
      }
      /*******************************************************************************/
      bool getResp(PGM_P rspStr) {
        u08 wait = 0;
        c08 msg[MAX_RX_LEN];
        do {
          _delay_ms(100);
          if (wait++ > TIME_OUT)
            return false;
        } while (uart->rxnum() < strlen(rspStr));
        uart->receive((u08*) msg, MAX_RX_LEN);
        return (strstr_P(msg, (rspStr)));
      }
      /*******************************************************************************/
      bool powerReset(void) {
        if (powerFET) {
          powerFET->disable();
          _delay_ms(1000);
          powerFET->enable();
          _delay_ms(500);
        }
        if (powerKey) {
          powerKey->setDir(OUT);
          powerKey->disable();
          _delay_ms(1500);
          powerKey->enable();
          _delay_ms(10);
          powerKey->setDir(IN);
          if (powerKey->disabled())
            return false;
        }
        return true;
      }
      /*******************************************************************************/
  }
  ;
}
using namespace SIM300;
#endif
