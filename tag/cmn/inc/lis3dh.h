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
typedef enum {
	ODR_POWER_DOWN = 0x0,
	ODR_1Hz = 0x1,
	ODR_10Hz = 0x2,
	ODR_25Hz = 0x3,
	ODR_50Hz = 0x4,
	ODR_100Hz = 0x5,
	ODR_200Hz = 0x6,
	ODR_400Hz = 0x7,
	ODR_1500Hz = 0x8,
	ODR_5000Hz = 0x9
} eDataRate;
//****************************************************************************************
typedef enum {
	// ODR Setting    1Hz   10Hz  25Hz  50Hz  100Hz 200Hz 400Hz 1.6kHz  5kHz
	HPC_00 = 0x0, //  0.02  0.2   0.5   1     2     4     8     32      100
	HPC_01 = 0x1, //  0.008 0.08  0.2   0.5   1     2     4     16      50
	HPC_10 = 0x2, //  0.004 0.04  0.1   0.2   0.5   1     2     8       25
	HPC_11 = 0x3 //   0.002 0.02  0.05  0.1   0.2   0.5   1     4       12
} eHighPassCutoff;
//****************************************************************************************
typedef enum {
	HPM_NORMAL_RESET = 0x0,
	HPM_REFERENCE = 0x1,
	HPM_NORMAL = 0x2,
	HPM_AUTORESET = 0x3
} eHighPassMode;
//****************************************************************************************
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
public:
	//****************************************************************************************
	Clis3dh(Ci2c* i2c, ePinState A0, u08 threshold) {
		sReg reg;
		u08 adr = 0x18;
		u08 dat;
		ok = false;
		adr = (adr | A0);
		this->i2c = i2c;
		i2c->setBitrate(400);
		i2c->setDevAdr(adr);
		setOperatingMode(false);
		setDataRate(ODR_25Hz);
		//setHighPassFilter(HPC_10, HPM_NORMAL_RESET,true);
		setInt1Threshold(threshold, 1);
		setInt1Config(false, false);
		// Clear the 4d interrupt
		//#dat = 0x08;
		//#write(offsetof(sReg,ctrl_reg5), 1, &dat);
		setFifoMode(FIFO_BYPASS_MODE);
		// The next piece of code is neede to stop the I2C bus from hanging Why? TODO
		read(0x00, sizeof(sReg), (u08*) &reg);
		if (reg.who_am_i == 0x33 && reg.ctrl_reg1 == 0x17) {
			ok = true;
		}
	}
	//****************************************************************************************
	void readReference(u08* ref) {
		read(offsetof(sReg,reference), 1, ref);
	}
	//****************************************************************************************
	void setDataRate(eDataRate odr) {
		u08 reg;
		read(offsetof(sReg,ctrl_reg1), 1, &reg);
		reg = (reg & 0x0f) | (odr << 4);
		write(offsetof(sReg,ctrl_reg1), 1, &reg);
	}
	//****************************************************************************************
	void setOperatingMode(bool lowPower = true, bool enableZaxis = true,
			bool enableYaxis = true, bool enableXaxis = true) {
		u08 reg;
		read(offsetof(sReg,ctrl_reg1), 1, &reg);
		reg = (reg & 0xf0)
				| (lowPower << 3 | enableZaxis << 2 | enableYaxis << 1
						| enableXaxis);
		write(offsetof(sReg,ctrl_reg1), 1, &reg);
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
	void getAccel(sAccel* dat) {
		read(offsetof(sReg,out_x_l), sizeof(sAccel), (u08*) dat);
	}
	//****************************************************************************************
	void getIntrCnt(u08* cnt) {
		read(offsetof(sReg,int_counter_reg), 1, cnt);
	}
	//****************************************************************************************
	void getIntrSrc(u08* reg) {
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
	void setInt1Config(bool enAOI = true, bool en6D = true, bool enZH = true,
			bool enZL = true, bool enYH = true, bool enYL = true, bool enXH =
					true, bool enXL = true) {
		u08 reg;
		reg = (enAOI << 7) | (en6D << 6) | (enZH << 5) | (enZL << 4)
				| (enYH << 3) | (enYL << 2) | (enXH << 1) | (enXL);
		write(offsetof(sReg,int1_cfg), 1, &reg);
	}
	//****************************************************************************************
	void setInt1Threshold(u08 threshold, u08 duration) {
		threshold = threshold & 0x7f;
		write(offsetof(sReg,int1_ths), 1, &threshold);
		duration = duration & 0x7f;
		write(offsetof(sReg,int1_duration), 1, &duration);
	}
	//****************************************************************************************
	void setHighPassFilter(eHighPassCutoff hpc, eHighPassMode hpm,
			bool enHihPassInt1 = true) {
		u08 reg;
		read(offsetof(sReg,ctrl_reg2), 1, &reg);
		reg = (reg & 0x0f) | (hpm << 6 || hpc << 4 || enHihPassInt1);
		write(offsetof(sReg,ctrl_reg2), 1, &reg);
	}
	//****************************************************************************************
	bool moved() {
		u08 reg;
		getIntrSrc(&reg);
		if ((reg & 0x3F) != (int1SrcReg & 0x3F)) {
			int1SrcReg = reg;
			return true;
		} else {
			int1SrcReg = reg;
			return false;
		}
	}
//****************************************************************************************
private:
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
};
//****************************************************************************************
}
#endif /* LIS3DH_H_ */
