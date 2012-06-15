#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "sim300.h"

#define DYNDNS_IP   "204.13.248.75"

#undef SMS_DEBUG
#define MDM_DEBUG_LEVEL 4
/*
 AT = OK, just to make the modem respond
 ATE1 = OK , enable the ECHO
 AT+CGATT=1 = OK, enable GPRS
 AT+CGDCONT=1,"IP","here_apn_provider" = OK, change for your APN
 AT+CSTT="here_apn_provider","user","pass" = Ok
 AT+CDNSORIP=0 = OK = to accept IP address(No domain name)
 AT+CIICR = OK, connect to the gprs using the apn
 AT+CIFSR = to check the IP address the provider give you
 AT+CLPORT="TCP","1720" = IF you using TCP, for UDP skip this one
 AT+CIPSTART="TCP","192.168.1.1","1720" = use your IP address and port it will respond CONNECT OK

 AT+CIPSTART="UDP","192.168.1.1","1720" = this is for UDP,
 */

PROGMEM char AT_OK[] = "\r\nOK\r\n";
PROGMEM char AT_RDY[] = ">";
PROGMEM char AT_NONE[] = "";
PROGMEM char AT_SEND_OK[] = "SEND OK";
PROGMEM char AT_CONNECT[] = "CONNECT OK";
PROGMEM char AT_ERROR[] = "ERROR";
PROGMEM char AT_DATA[] = "\r\n";
PROGMEM char SIM_RDY[] = "+CPIN: READY";

extern Cuart DbgUart;
/*******************************************************************************/
//Csim300::Csim300(Cuart * _pUart) {
//  usedns = false;
//  pUart = _pUart;
//  retry = MDM_RETRIES_VAL;
//  taskRetry = 0;
//  timeout = MDM_TIMEOUT_VAL;
//  mdmState = MDM_STARTUP;
//  cmdState = START;
//  signal_ok = false;
//  simcard_ok = false;
//  connect_ok = false;
//  registered_ok = false;
//  pwrState = 1;
//  initState = 1;
//  strcpy(serverIP, "");
//  strcpy(port, "");
//  serviceTime = 0;
//  atomicTime = 0;
//  failState = MDM_STARTUP;
//  error_cnt = 0;
//  smsrx = false;
//  smstx_en = true;
//  gprsrx = false;
//}
//
///*******************************************************************************/
//void Csim300::ServerSetIP(c08* _IP, c08 *_port, bool _usedns) {
//  usedns = _usedns;
//  strcpy(serverIP, _IP);
//  strcpy(port, _port);
//}
//
///*******************************************************************************/
//bool Csim300::DataToServer(c08* dat) {
//  if (mdmState == MDM_READY&& connect_ok == true && registered_ok == true && simcard_ok
//  == true && signal_ok == true) {
//    strcpy(txcmd, dat);
//    mdmState = MDM_IP_SEND;
//    return true;
//  } else {
//    return false;
//  }
//}
///*******************************************************************************/
//bool Csim300::SendSMS(char *PhoneNumber, char *Message) {
//  if (mdmState == MDM_READY && simcard_ok == true && smstx_en == true) {
//    mdmState = MDM_SEND_SMS;
//    strcpy(sms.phonenum, PhoneNumber);
//    strcpy(sms.message, Message);
//    return true;
//  } else {
//    if(simcard_ok!=true) {
//      DbgUart.sendStr_P(PSTR("\n\rSIM NOT YET REGISTERED"));
//      mdmState = MDM_STARTUP;
//      cmdState = START;
//    }
//    if(mdmState != MDM_READY) {
//      DbgUart.sendStr_P(PSTR("\n\rMODEM STATE WRONG"));
//      mdmState = MDM_STARTUP;
//      cmdState = START;
//    }
//    return false;
//  }
////  return true;
//}
///*******************************************************************************/
//bool Csim300::GetSignalQuality(void) {
//  if (mdmState == MDM_READY) {
//    mdmState = MDM_GET_SIGNAL_STRENGHT;
//    return true;
//  } else {
//    return false;
//  }
//}
///*******************************************************************************/
//bool Csim300::SIMCheckReady(void) {
//  if (mdmState == MDM_READY) {
//    mdmState = MDM_GET_SIMCARD_STATUS;
//    return true;
//  } else {
//    return false;
//  }
//}
//
//void Csim300::UpdateMdmStatus(void) {
//  if (mdmState == MDM_READY) {
//    mdmState = MDM_GET_SIMCARD_STATUS;
//  }
//}
//
///*
// bool CModem::SIMSetPin(c08* pin) {
// if (mdmState == MDM_READY) {
// strcpy(pincode, pin);
// mdmState = MDM_SET_SIMCARD_PIN;
// return true;
// } else {
// return false;
// }
// }
// */
//
///*******************************************************************************/
//eMdmState Csim300::GetStateModem(void) {
//  return mdmState;
//}
///******************************************************************************/
//void Csim300::setNextState(eMdmState nextState) {
//  //if (mdmState == MDM_READY) { // HACK
//  mdmState = nextState;
//  //}
//}
//
//void Csim300::clearTimer(void) {
//  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
//    isr_timer = 0;
//    atomicTime = 0;
//  }
//}
//
///*******************************************************************************/
//void Csim300::Service(void) {
//  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
//    atomicTime = isr_timer;
//    serviceTime = isr_timer;
//  }
//
//  if (serviceTime < SERVICE_INTERVAL) {
//    return;
//  }
//  serviceTime = 0;
//
//  timeout = MDM_TIMEOUT_VAL;
//  //GetUnSolicited();
//
//  if (taskRetry > MDM_RETRIES_VAL) {
//    taskRetry = 0;
//    mdmState = MDM_READY;
//  }
//  failState = MDM_STARTUP;
//
//  switch (mdmState) {
//    /*******************************************************************************/
//    case MDM_STARTUP:
//      switch (cmdState) {
//        case START:
//          retry = 0;
//          DbgUart.sendStr_P(PSTR("\n\rMDM_STARTUP"));
//          cmdState = RETRY;
//          initState = 1;
//          signal_ok = false;
//          simcard_ok = false;
//          connect_ok = false;
//          registered_ok = false;
//          break;
//        case RETRY:
//        case SEND:
//          if (Init()) {
//            mdmState = MDM_CONFIGURE;
//          }
//          break;
//      }
//      break;
//    case MDM_CONFIGURE:
//      strcpy_P(txcmd, PSTR("ATE0\r"));
//      if (HandleAtCmd(txcmd, AT_NONE)) {
//        mdmState = MDM_TEST_AT;
//      }
//      break;
//    case MDM_TEST_AT:
//      strcpy_P(txcmd, PSTR("AT\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_ADD_IPHEAD;
//      }
//      break;
//    case MDM_GET_TIME:
//      strcpy_P(txcmd, PSTR("AT+CCLK\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_READY;
//      }
//      break;
//    case MDM_ADD_IPHEAD:
//      strcpy_P(txcmd, PSTR("AT+CIPHEAD=1\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_SET_SMS_TEXT_MODE;
//      }
//      break;
//    case MDM_SET_SMS_TEXT_MODE:
//      strcpy_P(txcmd, PSTR("AT+CMGF=1\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_GET_SIGNAL_STRENGHT;
//        error_cnt = 0;
//      }
//      break;
//    case MDM_GET_SIGNAL_STRENGHT:
//      strcpy_P(txcmd, PSTR("AT+CSQ\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        pRx = rxmsg;
//        str = strsep(&pRx, ":");
//        //DbgUart.sendStr(str);
//        str = strsep(&pRx, ",");
//        //DbgUart.sendStr(str);
//        signalStrenght = atoi(str);
//        signal_ok = false;
//        if (signalStrenght == 99) {
//          error_cnt++;
//        } else if (signalStrenght > 5) {
//          signal_ok = true;
//          mdmState = MDM_GET_SIMCARD_STATUS;
//          error_cnt = 0;
//        } else {
//          error_cnt++;
//        }
//        if (error_cnt > 200) {
//          mdmState = MDM_FAILED;
//          DbgUart.sendStr_P(PSTR("\n\rSIGNAL STRENGHT TOO LOW!!"));
//        }
//      }
//      break;
//    case MDM_GET_SIMCARD_STATUS:
//      strcpy_P(txcmd, PSTR("AT+CPIN?\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        simcard_ok = false;
//        if (strstr_P(rxmsg, PSTR("+CPIN: READY"))) {
//          simcard_ok = true;
//          mdmState = MDM_IS_REGISTERED;
//          error_cnt = 0;
//        } else if (strstr_P(rxmsg, PSTR("+CPIN: SIM PIN"))) {
//          mdmState = MDM_SET_SIMCARD_PIN;
//          error_cnt = 0;
//        }
//      }
//      if (strstr_P(rxmsg, PSTR("+CME ERROR"))) {
//        mdmState = MDM_FAILED;
//        DbgUart.sendStr_P(PSTR("\n\rNO SIMCARD!!"));
//      }
//      break;
//    case MDM_SET_SIMCARD_PIN:
//      timeout = MDM_TIMEOUT_VAL * 5; //5seconds
//      strcpy_P(txcmd, PSTR("AT+CPIN="));
//      strcpy(pincode, "3470");
//      if (strlen(pincode) < 4) {
//        mdmState = MDM_FAILED; // TODO maybe goto a restart state?
//#if MDM_DEBUG_LEVEL > 2
//            DbgUart.sendStr_P(PSTR("\n\rNO VALID PIN SET"));
//#endif
//        break;
//      }
//      strcat(txcmd, pincode);
//      strcat_P(txcmd, PSTR("\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        simcard_ok = true;
//        mdmState = MDM_GET_SIMCARD_STATUS;
//      }
//      break;
//
//    case MDM_IS_REGISTERED:
//      strcpy_P(txcmd, PSTR("AT+CREG?\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        registered_ok = false;
//        if (strstr_P(rxmsg, PSTR("+CREG: 0,1"))) {
//          registered_ok = true;
//          mdmState = MDM_READY;
//        } else {
//          error_cnt++;
//          if (error_cnt > 200) {
//            mdmState = MDM_FAILED;
//            DbgUart.sendStr_P(PSTR("\n\rSIMCARD REGISTRATION FAILED!!"));
//          }
//        }
//      }
//      break;
//
//    case MDM_IP_STATUS:
//      strcpy_P(txcmd, PSTR("AT+CIPSTATUS\r"));
//      connect_ok = false;
//      if (HandleAtCmd(txcmd, AT_DATA)) {
//        if (strstr_P(rxmsg, AT_CONNECT)) {
//          mdmState = MDM_READY;
//          connect_ok = true;
//          DbgUart.sendStr("MODEM CONNECTED!!");
//        }
//        if (strstr_P(rxmsg, PSTR("INITIAL"))) {
//          mdmState = MDM_CONNECT_TO_SERVER;
//        } else if (strstr_P(rxmsg, PSTR("IP CLOSE"))) {
//          mdmState = MDM_START_IP;
//        } else if (strstr_P(rxmsg, PSTR("STATE: PDP DEACT"))) {
//          mdmState = MDM_IPSHUTDOWN;
//        } else {
//          mdmState = MDM_READY;
//        }
//      }
//      break;
//
//      /*******************************************************************************/
//    case MDM_READ_SMS:
//      strcpy_P(txcmd, PSTR("AT+CMGR="));
//      strcat(txcmd, sms.nr);
//      strcat_P(txcmd, PSTR("\r"));
//      if (HandleAtCmd(txcmd, AT_DATA)) {
//        strcpy(sms.message, rxmsg);
//        mdmState = MDM_READY;
//        smsrx = true;
//      }
//      break;
//
//    case MDM_SEND_SMS:
//      timeout = MDM_TIMEOUT_VAL * 5; //5seconds
//#ifdef SMS_DEBUG
//        mdmState = MDM_READY;
//        DbgUart.sendStr_P(PSTR("\n\r ## SMS SEND OK ##"));
//#else
//      strcpy_P(txcmd, PSTR("AT+CMGS=\""));
//      strcat(txcmd, sms.phonenum);
//      strcat_P(txcmd, PSTR("\"\r"));
//      if (HandleAtCmd(txcmd, AT_RDY)) {
//        pUart->sendStr(sms.message);
//        pUart->sendStr_P(PSTR("\x1A"));
//        mdmState = MDM_SMS_WAIT_SEND_OK;
//      } else if (cmdState == SEND) { //a timeout occured
//        pUart->sendStr_P(PSTR("\x1B")); // send an escape
//      }
//      break;
//
//    case MDM_SMS_WAIT_SEND_OK:
//      timeout = MDM_TIMEOUT_VAL * 5; //5seconds
//      if (GetAtResp(AT_OK)) {
//        DbgUart.sendStr_P(PSTR("\n\rSMS SEND OK"));
//        mdmState = MDM_READY;
//      } else if (cmdState == SEND) { //a timeout occured
//        DbgUart.sendStr_P(PSTR("\n\rSMS SEND ERROR"));
//        mdmState = MDM_READY;
//        //taskRetry++;
//      }
//#endif
//      break;
//
//    case MDM_CONNECT_TO_SERVER:
//      strcpy_P(txcmd, PSTR("AT+CGDCONT=1,\"IP\",\"internet\"\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_SET_SHOW_REMOTE_IP;
//      }
//      break;
//    case MDM_SET_SHOW_REMOTE_IP:
//      strcpy_P(txcmd, PSTR("AT+CIPSRIP=0\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_SET_DNS_CONNECT;
//      }
//      break;
//    case MDM_SET_DNS_CONNECT:
//      failState = MDM_IPSHUTDOWN;
//      if (usedns) {
//        strcpy_P(txcmd, PSTR("AT+CDNSORIP=1\r"));
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          mdmState = MDM_SET_DNS_CONFIG;
//        }
//      } else {
//        strcpy_P(txcmd, PSTR("AT+CDNSORIP=0\r"));
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          mdmState = MDM_SET_TCP_PORT;
//        }
//      }
//      break;
//    case MDM_SET_DNS_CONFIG:
//      failState = MDM_IPSHUTDOWN;
//      strcpy_P(txcmd, PSTR("AT+CDNSCFG=\""));
//      strcat(txcmd, DYNDNS_IP);
//      strcat_P(txcmd, PSTR("\"\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_SET_TCP_PORT;
//      }
//      break;
//    case MDM_SET_TCP_PORT:
//      failState = MDM_IPSHUTDOWN;
//
//      strcpy_P(txcmd, PSTR("AT+CLPORT=\"TCP\",\""));
//      strcat(txcmd, port);
//      strcat(txcmd, "\"\r");
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_START_TCP_TASK;
//      }
//      break;
//    case MDM_START_TCP_TASK:
//      failState = MDM_IPSHUTDOWN;
//      timeout = MDM_TIMEOUT_VAL * 30; //30 sekondes
//      strcpy_P(txcmd, PSTR("AT+CSTT=\"internet\",\"\",\"\"\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_ACTIVATE_PDP;
//      }
//      break;
//    case MDM_ACTIVATE_PDP:
//      failState = MDM_IPSHUTDOWN;
//      timeout = MDM_TIMEOUT_VAL * 30; //30 sekondes
//      strcpy_P(txcmd, PSTR("AT+CIICR\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_GET_IP;
//      }
//      break;
//    case MDM_GET_IP:
//      failState = MDM_IPSHUTDOWN;
//      timeout = MDM_TIMEOUT_VAL * 10; //10 sekondes
//      strcpy_P(txcmd, PSTR("AT+CIFSR\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_START_IP;
//      }
//      if (cmdState == SEND) {
//        DbgUart.sendStr(rxmsg);
//        cmdState = START;
//        mdmState = MDM_START_IP;
//      }
//      break;
//    case MDM_START_IP:
//      timeout = MDM_TIMEOUT_VAL * 30; //20 sekondes
//      failState = MDM_IPSHUTDOWN;
//      strcpy(txcmd, "AT+CIPSTART=\"TCP\",\"");
//      strcat(txcmd, serverIP);
//      strcat(txcmd, "\",\"");
//      strcat(txcmd, port);
//      strcat(txcmd, "\"\r");
//      connect_ok = false;
//      if (HandleAtCmd(txcmd, AT_CONNECT)) {
//        connect_ok = true;
//        DbgUart.sendStr("MODEM CONNECTED!!");
//        mdmState = MDM_IP_STATUS;
//      }
//      if (cmdState == SEND) {
//        cmdState = START;
//        mdmState = MDM_IP_STATUS;
//      }
//      break;
//    case MDM_IP_SEND:
//      failState = MDM_START_IP;
//      if (HandleAtCmd("AT+CIPSEND\r", AT_RDY)) {
//        mdmState = MDM_IPSEND_VERIFY;
//        cmdState = START;
//        DbgUart.sendStr(txcmd);
//        pUart->sendStr(txcmd);
//        pUart->sendStr_P(PSTR("\x1A\r"));
//      }
//      break;
//    case MDM_IPSEND_VERIFY:
//      timeout = MDM_TIMEOUT_VAL * 30;
//      failState = MDM_IPSHUTDOWN;
//      if (GetAtResp(AT_SEND_OK)) {
//        DbgUart.sendStr("\n\rSUKSES");
//        mdmState = MDM_READY;
//      } else if (cmdState == SEND) { //a timeout occured
//#if MDM_DEBUG_LEVEL > 2
//          DbgUart.sendStr_P(PSTR("\n\rDATA SEND ERROR"));
//#endif
//        mdmState = MDM_IP_SEND;
//        //taskRetry++;
//      }
//      break;
//
//    case MDM_IPSHUTDOWN:
//      strcpy_P(txcmd, PSTR("AT+CIPSHUT\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_CONNECT_TO_SERVER;
//      }
//      connect_ok = false;
//      break;
//
//    case MDM_GET_BALANCE:
//      timeout = MDM_TIMEOUT_VAL * 10; //10 sekondes
//      strcpy_P(txcmd, PSTR("ATD*100#\r"));
//      if (HandleAtCmd(txcmd, AT_DATA)) {
//        mdmState = MDM_READY;
//        DbgUart.sendStr(rxmsg);
//      }
//      break;
//
//    case MDM_FAILED:
//      mdmState = MDM_STARTUP;
//      cmdState = START;
//      break;
//
//    case MDM_READY:
//      break;
//
//    default:
//      break;
//  }
//}
///*******************************************************************************/
//bool Csim300::HandleAtCmd(c08 *str, PGM_P rspStr) {
//  bool ret;
//  ret = false;
//  switch (cmdState) {
//    case START:
//      retry = 0;
//      clearTimer();
//    case SEND:
//      memset(rxmsg, 0, sizeof(rxmsg));
//      DbgUart.sendStr(str);
//      pUart->sendStr(str);
//      cmdState = RETRY;
//      clearTimer();
//      retry++;
//      break;
//    case RETRY:
//      ret = GetAtResp(rspStr);
//      break;
//  }
//  return ret;
//}
