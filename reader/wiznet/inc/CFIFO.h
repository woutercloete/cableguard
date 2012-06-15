/*
 * CFIFO.h
 *
 *  Created on: 02 Apr 2009
 *      Author: Wouter
 */

#ifndef CFIFO_H_
#define CFIFO_H_

#include "avrlibtypes.h"

class CFIFO {
  private:
	  volatile u16 headIndex;
	  volatile u16 tailIndex;
	  volatile u08* buffer;
    u16 size;
    u08 healthy;
  public:
    CFIFO(void);
    u08 setBufSize(u16 size);
    u16 add(u08* src, u16 numBytes);
    u16 remove(u08* dst, u16 numBytes);
    u16 peek(c08* dst);
    u16 space(void);
    u08 empty(void);
    void clear(void);
    u16 received(void);
};
#endif /* CFIFO_H_ */
