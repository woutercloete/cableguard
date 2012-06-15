/*
 * CFIFO.cpp
 *
 *  Created on: 02 Apr 2009
 *      Author: Wouter
 */
#include <stdlib.h>
#include "CFIFO.h"

CFIFO::CFIFO(void) {
  size = 0;
  headIndex = 0;
  tailIndex = 0;
  healthy = true;
  buffer = 0;
}

u08 CFIFO::setBufSize(u16 _size) {
  buffer = (u08*) malloc(_size);
  if (buffer == NULL) {
    healthy = false;
    return false;
  }
  size = _size;
  return true;
}

u16 CFIFO::space(void) {
  u16 used = 0;
  if (headIndex >= tailIndex)
    used = headIndex - tailIndex;
  else
    used = size + headIndex - tailIndex;
  return (size-used-1);
}

u16 CFIFO::received(void) {
  u16 used = 0;
  if (headIndex >= tailIndex)
    used = headIndex - tailIndex;
  else
    used = size + headIndex - tailIndex;
  return (used);
}

u16 CFIFO::add(u08 *src, u16 numBytes) {
  u16 cnt = 0;
  u16 used = 0;
  if (headIndex >= tailIndex)
    used = headIndex - tailIndex;
  else
    used = size + headIndex - tailIndex;
  while (cnt < numBytes && used < (size - 1)) {
    buffer[headIndex] = *src;
    headIndex = (headIndex + 1) % size;
    src++;
    used++;
    cnt++;
  }
  return cnt;
}

u08 CFIFO::empty(void){
  return (space() == (size-1));
}

void CFIFO::clear(void){
	tailIndex = 0;
	headIndex = 0;
}


u16 CFIFO::remove(u08 *dst, u16 numBytes) {
  u16 cnt = 0;
  u16 used = 0;
  if (headIndex >= tailIndex)
    used = headIndex - tailIndex;
  else
    used = size + headIndex - tailIndex;
  while (cnt < numBytes && used > 0) {
    *dst = buffer[tailIndex];
    tailIndex = (tailIndex + 1) % size;
    dst++;
    used--;
    cnt++;
  }
  return cnt;
}

u16 CFIFO::peek(c08 *dst) {
  u16 cnt = 0;
  u16 used = 0;
  u16 tail;
  if (headIndex >= tailIndex)
    used = headIndex - tailIndex;
  else
    used = size + headIndex - tailIndex;

  tail = tailIndex;

  while (used > 0) {
    *dst = buffer[tail++];
    dst++;
    used--;
    cnt++;
  }
  return cnt;
}

