//****************************************************************************************
// FIR filter class
//
//   Created : 3 June 2012
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
//****************************************************************************************
#ifndef FIR_H_
#define FIR_H_
/****************************************************************************************/
#include <stdlib.h>
#include <math.h>
/****************************************************************************************/
#include "circular.h"
/****************************************************************************************/
namespace FIR {
  typedef struct {
  } eWeights;
  //****************************************************************************************
  class Cfir {
      Tcircular<u08> delayLine;
      f32 weight[8];
    public:
      Cfir() {
      }
      //****************************************************************************************
      void fill(u08 rssi) {
        delayLine.fill(&rssi);
      }
      //****************************************************************************************
      void setSize(u08 size) {
        delayLine.setBufSize(size);
//        weight = (f32*) malloc(size * sizeof(f32));
//        while (weight == NULL) {
//        };
      }
      //****************************************************************************************
      void setWeight(f32 _weight, bool first) {
        static u08 weightNum = 0;
        if (first)
          weightNum = 0;
        if (weightNum < delayLine.size)
          weight[weightNum++] = _weight;
      }
      //****************************************************************************************
      void clock(u08* in, u08* out) {
        *out = 0;
        u08 tap;
        f32 signal = 0.0;
        delayLine.add(in);
        delayLine.read(&tap, true);
        for (u08 cnt = 0; cnt < delayLine.size; cnt++) {
          signal = signal + (f32) tap * weight[cnt];
          delayLine.read(&tap, false);
        }
        *out = (u08) round(signal);
      }
      //****************************************************************************************
  };
//****************************************************************************************
}
#endif /* FIR_H_ */
