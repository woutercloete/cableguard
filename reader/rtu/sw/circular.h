//****************************************************************************************
// The interface to the LIS3DH.
// http://www.st.com/internet/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/DATASHEET/CD00274221.pdf
// http://www.st.com/internet/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/APPLICATION_NOTE/CD00290365.pdf
//
//   Created : 8 May 2012
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
//****************************************************************************************
#ifndef CIRCULAR_H_
#define CIRCULAR_H_
/****************************************************************************************/
#include <util/atomic.h>
/****************************************************************************************/
#include "types.h"
/****************************************************************************************/
template<class T>
class Tcircular {
  private:
    u16 headIndex;
    u16 writeIndex;
    u16 readIndex;
    void* buffer;
  public:
    u16 size;
    Tcircular(void);
    bool setBufSize(u16 numBlocks);
    u16 add(T* src);
    void read(T* dst);
    void read(T* _dst, bool first);
    void fill(T* val);
};
/****************************************************************************************/
template<class T>
Tcircular<T>::Tcircular(void) {
  size = 0;
  headIndex = 0;
  writeIndex = 0;
  buffer = 0;
}
/****************************************************************************************/
template<class T>
bool Tcircular<T>::setBufSize(u16 numBlocks) {
  if (numBlocks <= 255) {
    buffer = malloc(numBlocks * sizeof(T));
  }
  while (buffer == NULL) {
  }
  size = numBlocks;
  return true;
}
/****************************************************************************************/
template<class T>
u16 Tcircular<T>::add(T* _src) {
  void* dst;
  void* src = _src;
  u16 cnt = 0;
  dst = (void*) ((u16) buffer + (writeIndex * sizeof(T)));
  memcpy(dst, src, sizeof(T));
  writeIndex = (writeIndex + 1) % size;
  return cnt;
}
/****************************************************************************************/
template<class T>
void Tcircular<T>::read(T* _dst) {
  void* dst = _dst;
  void* src;
  u16 cnt = 0;
  readIndex = writeIndex;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    for (cnt = 0; cnt < size; cnt++) {
      readIndex = size - ((readIndex - 1) % size);
      src = (void*) ((u16) buffer + (readIndex * sizeof(T)));
      memcpy(dst, src, sizeof(T));
      dst = (void*) ((u16) dst + sizeof(T));
      cnt++;
    }
  }
}
/****************************************************************************************/
template<class T>
void Tcircular<T>::read(T* _dst, bool first) {
  void* src;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (first) {
      readIndex = writeIndex;
    }
    if (readIndex == 0)
      readIndex = size - 1;
    else
      readIndex--;
    src = (void*) ((u16) buffer + (readIndex * sizeof(T)));
    memcpy(_dst, src, sizeof(T));
  }
}
/****************************************************************************************/
template<class T>
void Tcircular<T>::fill(T* val) {
  void* dst = buffer;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    for (u08 cnt = 0; cnt < size; cnt++) {
      memcpy(dst, val, sizeof(T));
      dst = (void*) ((u16) dst + sizeof(T));
    }
  }
  writeIndex = 0;
}
/****************************************************************************************/
#endif /* CIRCULAR_H_ */
