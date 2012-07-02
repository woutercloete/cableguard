/****************************************************************************************/
// The Csocket class maintains a socket connection to a server.
//
//   Created : 15 March 2012
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
/****************************************************************************************/
#ifndef CSOCKET_H
#define CSOCKET_H
/****************************************************************************************/
#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
/****************************************************************************************/
#include "common.h"
#include "CFIFO.h"
#include "pin.h"
#include "Cserial.h"
#include "scheduler.h"
#include "payload.h"
/****************************************************************************************/
using namespace XREADER;
/****************************************************************************************/
namespace CSOCKET {
  const u32 LIFETIME = (5); // 5 Second timeout
}
/****************************************************************************************/
class Csocket: public Cserial {
    u08 socketNr;
    u16 port;
    bool print;
    sIP dst;
    u32 idleTime;
    Csignal signal;
    u08* buf;
    u16 bufSize;
    bool autoclose;
  public:
    Csocket(u32 socketNr, u16 port, sIP dstIP, u16 bufSize);
    void service(void);
    u16 send(u08* buffer, u16 nBytes);
    u16 receive(u08* buffer, u16 nBytes);
    bool start(void);
    void setDst(sIP _dst) {
      dst = _dst;
      disconnectSocket(socketNr);
      closeSocket(socketNr);
    }
    void enableAutoclose();
    void disableAutoclose() {
      autoclose = false;
    }
};
/****************************************************************************************/
#endif

