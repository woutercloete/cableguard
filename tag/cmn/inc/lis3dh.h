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
#ifndef LIS3DH_H_
#define LIS3DH_H_
//****************************************************************************************
#include <stddef.h>
#include <string.h>
//****************************************************************************************
#include "i2c.h"
//****************************************************************************************
using namespace I2C;
//****************************************************************************************
namespace LIS3DH {
  typedef enum {
    FIFO_BYPASS_MODE = 0x00,
      FIFO_MODE = 0x01,
      FIFO_STREAM_MODE = 0x02,
      FIFO_TRIGGER_MODE = 0x03,
      FIFO_DISABLE = 0x04
  } eFifoMode;
  //****************************************************************************************
  typedef struct {
      u08 reserveda[7];
      u08 status_reg_aux;
      u08 out_adc1_l;
      u08 out_adc1_h;
      u08 out_adc2_l;
      u08 out_adc2_h;
      u08 out_adc3_l;
      u08 out_adc3_h;
      u08 int_counter_reg;
      u08 who_am_i;
      u08 reservedb[15];
      u08 temp_cfg_reg;
      u08 ctrl_reg1;
      u08 ctrl_reg2;
      u08 ctrl_reg3;
      u08 ctrl_reg4;
      u08 ctrl_reg5;
      u08 ctrl_reg6;
      u08 reference;
      u08 status_reg2;
      u08 out_x_l;
      u08 out_x_h;
      u08 out_y_l;
      u08 out_y_h;
      u08 out_z_l;
      u08 out_z_h;
      u08 fifo_ctrl_reg;
      u08 fifo_src_reg;
      u08 int1_cfg;
      u08 int1_source;
      u08 int1_ths;
      u08 int1_duration;
      u08 reservedc[4];
      u08 click_cfg;
      u08 click_src;
      u08 click_ths;
      u08 time_limit;
      u08 time_latency;
      u08 time_window;
  } sReg;
  //****************************************************************************************
  typedef struct {
      s16 x;
      s16 y;
      s16 z;
  } sAccel;
  //****************************************************************************************
  class Clis3dh {
      Ci2c* i2c;
      bool ok;
      u08 int1SrcReg;
      //****************************************************************************************
      bool read(u08 adr, u08 len, u08* dat) {
        if (len > 1)
          adr |= 0x80;
        i2c->tx(1, &adr);
        return (i2c->rx(len, dat));
      }
      //****************************************************************************************
      void write(u08 adr, u08 len, u08* dat) {
        u08 buf[8];
        if (len > 1)
          adr |= 0x80;
        buf[0] = adr;
        memcpy(&buf[1], dat, len);
        i2c->tx(len + 1, buf);
      }
    public:
      //****************************************************************************************
      Clis3dh(Ci2c* i2c, ePinState A0) {
        u08 adr = 0x18;
        u08 dat;
        sReg reg;
        ok = false;
        adr = (adr | A0);
        this->i2c = i2c;
        i2c->setBitrate(400);
        i2c->setDevAdr(adr);
        // Config Data Rate
        dat = 0x37;
        //dat = 0x0f;
        write(offsetof(sReg,ctrl_reg1), 1, &dat);
        // Enable the AOI interrupt
        dat = 0x01;
        write(offsetof(sReg,ctrl_reg2), 1, &dat);
        dat = 0x00;
        write(offsetof(sReg,ctrl_reg3), 1, &dat);
        dat = 0x00;
        write(offsetof(sReg,ctrl_reg4), 1, &dat);
        // Enable the AOI interrupt
        dat = 0xff;
        //write(offsetof(sReg,ctrl_reg6), 1, &dat);
        // Set the 6D interrupt threshold
        dat = 2;
        write(offsetof(sReg,int1_ths), 1, &dat);
        // Set the 6D interrupt duration
        dat = 1;
        write(offsetof(sReg,int1_duration), 1, &dat);
        // Enable the 6D interrupt
        dat = 0x2f;
        write(offsetof(sReg,int1_cfg), 1, &dat);
        // Clear the 4d interrupt
        dat = 0x08;
        write(offsetof(sReg,ctrl_reg5), 1, &dat);
        // Setup the FIFO
        dat = 0x00;
        write(offsetof(sReg,fifo_ctrl_reg), 1, &dat);
        // Read the full register set
        read(0x00, sizeof(sReg), (u08*) &reg);
        if (reg.who_am_i == 0x33 && reg.ctrl_reg1 == 0x17) {
          ok = true;
        }
      }
      //****************************************************************************************
      void readAccel(sAccel* dat) {
        read(offsetof(sReg,out_x_l), sizeof(sAccel), (u08*) dat);
      }
      //****************************************************************************************
      void readIntrCntReg(u08* cnt) {
        read(offsetof(sReg,int_counter_reg), 1, cnt);
      }
      //****************************************************************************************
      void readIntrSrcReg(u08* reg) {
        read(offsetof(sReg,int1_source), 1, reg);
      }
      //****************************************************************************************
      bool fifoEmpty(void) {
        u08 reg;
        read(offsetof(sReg,fifo_src_reg), 1, &reg);
        return ((reg & 0x20) == 0x20);
      }
      //****************************************************************************************
      bool fifoWatermark(void) {
        u08 reg;
        read(offsetof(sReg,fifo_src_reg), 1, &reg);
        return ((reg & 0x80) == 0x80);
      }
      //****************************************************************************************
      void setFifoMode(eFifoMode mode) {
        u08 reg;
        read(offsetof(sReg,fifo_ctrl_reg), 1, &reg);
        reg = (reg & 0x2f) | (mode << 6);
        write(offsetof(sReg,fifo_ctrl_reg), 1, &reg);
      }
      //****************************************************************************************
      bool moved() {
        u08 reg;
        readIntrSrcReg(&reg);
        if ((reg & 0x3F) != (int1SrcReg & 0x3F)) {
          int1SrcReg = reg;
          return true;
        } else {
          int1SrcReg = reg;
          return false;
        }
      }
  };
//****************************************************************************************
}
#endif /* LIS3DH_H_ */
