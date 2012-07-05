//****************************************************************************************
// Generic I2C interface.
//
//   Created : 2 May 2012
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
//****************************************************************************************
#ifndef I2C_H
#define I2C_H
//****************************************************************************************
#include <avr/io.h>
//****************************************************************************************
#include "common.h"
#include "avrlibtypes.h"
#include "avrlibdefs.h"
#include "pin.h"
#include "fifo.h"
//****************************************************************************************
// Master
const u08 TW_START = 0x08;
const u08 TW_REP_START = 0x10;
// Master Transmitter
const u08 TW_MT_SLA_ACK = 0x18;
const u08 TW_MT_SLA_NACK = 0x20;
const u08 TW_MT_DATA_ACK = 0x28;
const u08 TW_MT_DATA_NACK = 0x30;
const u08 TW_MT_ARB_LOST = 0x38;
// Master Receiver
const u08 TW_MR_ARB_LOST = 0x38;
const u08 TW_MR_SLA_ACK = 0x40;
const u08 TW_MR_SLA_NACK = 0x48;
const u08 TW_MR_DATA_ACK = 0x50;
const u08 TW_MR_DATA_NACK = 0x58;
// Slave Transmitter
const u08 TW_ST_SLA_ACK = 0xA8;
const u08 TW_ST_ARB_LOST_SLA_ACK = 0xB0;
const u08 TW_ST_DATA_ACK = 0xB8;
const u08 TW_ST_DATA_NACK = 0xC0;
const u08 TW_ST_LAST_DATA = 0xC8;
// Slave Receiver
const u08 TW_SR_SLA_ACK = 0x60;
const u08 TW_SR_ARB_LOST_SLA_ACK = 0x68;
const u08 TW_SR_GCALL_ACK = 0x70;
const u08 TW_SR_ARB_LOST_GCALL_ACK = 0x78;
const u08 TW_SR_DATA_ACK = 0x80;
const u08 TW_SR_DATA_NACK = 0x88;
const u08 TW_SR_GCALL_DATA_ACK = 0x90;
const u08 TW_SR_GCALL_DATA_NACK = 0x98;
const u08 TW_SR_STOP = 0xA0;
// Misc
const u08 TW_NO_INFO = 0xF8;
const u08 TW_BUS_ERROR = 0x00;
// defines and constants
const u08 TWCR_CMD_MASK = 0x0F;
const u08 TWSR_STATUS_MASK = 0xF8;
const u08 I2C_SEND_DATA_BUFFER_SIZE = 0x10;
const u08 I2C_RECEIVE_DATA_BUFFER_SIZE = 0x10;
//****************************************************************************************
namespace I2C {
  typedef struct {
      u08 twbr;
      u08 twsr;
      u08 twar;
      u08 twdr;
      u08 twcr;
  } sReg;
//****************************************************************************************
  typedef enum {
    IDLE = 0, BUSY = 1, MASTER_TX = 2, MASTER_RX = 3, SLAVE_TX = 4, SLAVE_RX = 5, MASTER_TX_NO_STOP
  } eState;
//****************************************************************************************
  class Ci2c {
    private:
      volatile sReg* reg;
      static eState mode;
      bool txStop;
      bool error;
      u08 cntrl;
      u08 rxLength;
      Tfifo<u08> txFifo;
      void (*slaveReceive)(u08 receiveDataLength, u08* recieveData);
      u08 (*slaveTransmit)(u08 transmitDataLengthMax, u08* transmitData);
      /****************************************************************************************/
      void sendStart(void) {
        reg->twcr = (reg->twcr & TWCR_CMD_MASK) | BV(TWINT) | BV(TWSTA);
      }
      /****************************************************************************************/
      void sendStop(void) {
        // transmit stop condition
        // leave with TWEA on for slave receiving
        reg->twcr = (reg->twcr & TWCR_CMD_MASK) | BV(TWINT) | BV(TWEA) | BV(TWSTO);
      }
      /****************************************************************************************/
      void clearTWINT(void) {
        reg->twcr = (reg->twcr & TWCR_CMD_MASK) | BV(TWINT);
      }
      /****************************************************************************************/
      void waitForComplete(void) {
        while (!(reg->twcr & BV(TWINT))) {
        };
      }
      //****************************************************************************************
      void sendByte(u08 data) {
        // Save data to the TWDR
        reg->twdr = data;
        // begin send
        reg->twcr = (reg->twcr & TWCR_CMD_MASK) | BV(TWINT);
      }
      //****************************************************************************************
      void receiveByte(bool ackFlag) {
        // begin receive over i2c
        if (ackFlag) {
          // ackFlag = TRUE: ACK the received data
          reg->twcr = (reg->twcr & TWCR_CMD_MASK) | BV(TWINT) | BV(TWEA);
        } else {
          // ackFlag = FALSE: NACK the received data
          reg->twcr = (reg->twcr & TWCR_CMD_MASK) | BV(TWINT);
        }
      }
      /****************************************************************************************/
      u08 getReceivedByte(void) {
        // Retrieve received data byte from i2c TWDR
        return (reg->twdr);
      }
      /****************************************************************************************/
      u08 getStatus(void) {
        // retrieve current i2c status from i2c TWSR
        return (reg->twsr);
      }
      /****************************************************************************************/
    public:
      u08 devAdr;
      Tfifo<u08> rxFifo;
      static Ci2c* activeDevice;
      Ci2c(u16 baseAdr, u08 txBufSize, u08 rxBufSize) {
        reg = (volatile sReg*) baseAdr;
        devAdr = 0x0;
        txFifo.setBufSize(txBufSize);
        rxFifo.setBufSize(rxBufSize);
        // clear SlaveReceive and SlaveTransmit handler to null
        slaveReceive = 0;
        slaveTransmit = 0;
        // enable TWI (two-wire interface)
        BIT_SET_HI(reg->twcr, TWEN);
        mode = IDLE;
        // enable TWI interrupt and slave address ACK
        //BIT_SET_HI(reg->twcr, TWIE);
        BIT_SET_HI(reg->twcr, TWEA);
        sei();
      }
      /****************************************************************************************/
      // Standard I2C bit rates are:
      // 100KHz for slow speed
      // 400KHz for high speed
      void setBitrate(u16 bitrateKHz) {
        u08 bitrate_div;
        // calculate bitrate division
        bitrate_div = ((F_CPU / 1000l) / bitrateKHz);
        if (bitrate_div >= 16)
          bitrate_div = (bitrate_div - 16) / 2;
        reg->twbr = bitrate_div;
      }
      /****************************************************************************************/
      void setDevAdr(u08 _devAdr) {
        this->devAdr = _devAdr;
      }
      /****************************************************************************************/
      void setLocalDeviceAddr(u08 deviceAddr, u08 genCallEn) {
        // set local device address (used in slave mode only)
        reg->twar = ((deviceAddr & 0xFE) | (genCallEn ? 1 : 0));
      }
      /****************************************************************************************/
      void setSlaveRxHandler(void (*i2cSlaveRx_func)(u08 rxDataLength, u08* rxData)) {
        slaveReceive = i2cSlaveRx_func;
      }
      /****************************************************************************************/
      void setSlaveTxHandler(u08 (*i2cSlaveTx_func)(u08 txDataLengthMax, u08* txData)) {
        slaveTransmit = i2cSlaveTx_func;
      }
      /****************************************************************************************/
      void tx(u08 length, u08 *data) {
        // wait for interface to be ready
        while (mode) {
          service();
        };
        activeDevice = this;
        mode = MASTER_TX;
        // save data
        cntrl = ((devAdr << 1) & 0xFE);
        txFifo.clear();
        txFifo.add(data, length);
        // send start condition
        sendStart();
      }
      /****************************************************************************************/
      bool rx(u08 length, u08* data) {
        // wait for interface to be ready
        while (mode) {
          service();
        };
        activeDevice = this;
        // set state
        mode = MASTER_RX;
        error = false;
        // save data
        cntrl = ((devAdr << 1) | 0x01);// RW set: read operation
        rxFifo.clear();
        rxLength = length;
        // send start condition
        sendStart();
        // wait for data
        while (mode) {
          service();
        };
        // return data
        rxFifo.remove(data, length);
        return !error;
      }
      /****************************************************************************************/
      eState getState(void) {
        return mode;
      }
      /****************************************************************************************/
      void service(void) {
        u08 dat;
        // read status bits
        u08 status = reg->twsr & TWSR_STATUS_MASK;
        switch (status) {
          // Master General
          case TW_START: // 0x08: Sent start condition
          case TW_REP_START: // 0x10: Sent repeated start condition
            // send device address
            sendByte(cntrl);
            break;
            // Master Transmitter & Receiver status codes
          case TW_MT_SLA_ACK: // 0x18: Slave address acknowledged
          case TW_MT_DATA_ACK: // 0x28: Data acknowledged
            if (txFifo.empty()) {
              // transmit stop condition, enable SLA ACK
              sendStop();
              // set mode
              mode = IDLE;
            } else {
              // send data
              txFifo.remove(&dat, 1);
              sendByte(dat);
            }
            break;
          case TW_MR_DATA_NACK: // 0x58: Data received, NACK reply issued
            // store final received data byte
            dat = reg->twdr;
            rxFifo.add(&dat, 1);
            sendStop();
            // set mode
            mode = IDLE;
            break;
          case TW_MR_SLA_NACK: // 0x48: Slave address not acknowledged
          case TW_MT_SLA_NACK: // 0x20: Slave address not acknowledged
          case TW_MT_DATA_NACK: // 0x30: Data not acknowledged
            // transmit stop condition, enable SLA ACK
            sendStop();
            // set mode
            mode = IDLE;
            error = true;
            break;
          case TW_MT_ARB_LOST: // 0x38: Bus arbitration lost
            //case TW_MR_ARB_LOST:        // 0x38: Bus arbitration lost
            // release bus
            reg->twcr = (reg->twcr & TWCR_CMD_MASK) | BV(TWINT);
            // set mode
            mode = IDLE;
            error = true;
            // release bus and transmit start when bus is free
            //outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTA));
            break;
          case TW_MR_DATA_ACK: // 0x50: Data acknowledged
            // store received data byte
            dat = reg->twdr;
            rxFifo.add(&dat, 1);
            rxLength--;
            // fall-through to see if more bytes will be received
          case TW_MR_SLA_ACK: // 0x40: Slave address acknowledged
            if (rxLength > 0)
              // data byte will be received, reply with ACK (more bytes in transfer)
              receiveByte(true);
            else
              // data byte will be received, reply with NACK (final byte in transfer)
              receiveByte(false);
            break;
            // Slave Receiver status codes
          case TW_SR_SLA_ACK: // 0x60: own SLA+W has been received, ACK has been returned
          case TW_SR_ARB_LOST_SLA_ACK: // 0x68: own SLA+W has been received, ACK has been returned
          case TW_SR_GCALL_ACK: // 0x70:     GCA+W has been received, ACK has been returned
          case TW_SR_ARB_LOST_GCALL_ACK: // 0x78:     GCA+W has been received, ACK has been returned
            // we are being addressed as slave for writing (data will be received from master)
            // set mode
            mode = SLAVE_RX;
            // prepare buffer
            // receive data byte and return ACK
            reg->twcr = (reg->twcr & TWCR_CMD_MASK) | BV(TWINT) | BV(TWEA);
            break;
          case TW_SR_DATA_ACK: // 0x80: data byte has been received, ACK has been returned
          case TW_SR_GCALL_DATA_ACK: // 0x90: data byte has been received, ACK has been returned
            // get previously received data byte
            dat = reg->twdr;
            rxFifo.add(&dat, 1);
            // check receive buffer status
            if (rxFifo.space() > 1) {
              // receive data byte and return ACK
              receiveByte(true);
            } else {
              // receive data byte and return NACK
              receiveByte(false);
            }
            break;
          case TW_SR_DATA_NACK: // 0x88: data byte has been received, NACK has been returned
          case TW_SR_GCALL_DATA_NACK: // 0x98: data byte has been received, NACK has been returned
            // receive data byte and return NACK
            receiveByte(false);
            break;
          case TW_SR_STOP: // 0xA0: STOP or REPEATED START has been received while addressed as slave
            // switch to SR mode with SLA ACK
            reg->twcr = (reg->twcr & TWCR_CMD_MASK) | BV(TWINT) | BV(TWEA);
            // i2c receive is complete, call CI2C::slaveReceive
            //if (slaveReceive)
            //slaveReceive(receiveDataIndex, receiveData);
            // set mode
            mode = IDLE;
            break;
            // Slave Transmitter
          case TW_ST_SLA_ACK: // 0xA8: own SLA+R has been received, ACK has been returned
          case TW_ST_ARB_LOST_SLA_ACK: // 0xB0:     GCA+R has been received, ACK has been returned
            // we are being addressed as slave for reading (data must be transmitted back to master)
            // set mode
            mode = SLAVE_TX;
            // request data from application
            //if (slaveTransmit)
            //sendDataLength = slaveTransmit(I2C_SEND_DATA_BUFFER_SIZE, sendData);
            // reset data index
            //sendDataIndex = 0;
            // fall-through to transmit first data byte
          case TW_ST_DATA_ACK: // 0xB8: data byte has been transmitted, ACK has been received
            // transmit data byte
            txFifo.remove(&dat, 1);
            reg->twdr = dat;
            if (txFifo.used() > 0)
              // expect ACK to data byte
              reg->twcr = (reg->twcr & TWCR_CMD_MASK) | BV(TWINT) | BV(TWEA);
            else
              // expect NACK to data byte
              reg->twcr = (reg->twcr & TWCR_CMD_MASK) | BV(TWINT);
            break;
          case TW_ST_DATA_NACK: // 0xC0: data byte has been transmitted, NACK has been received
          case TW_ST_LAST_DATA: // 0xC8:
            // all done
            // switch to open slave
            reg->twcr = (reg->twcr & TWCR_CMD_MASK) | BV(TWINT) | BV(TWEA);
            // set mode
            mode = IDLE;
            break;
          case TW_NO_INFO: // 0xF8: No relevant state information
            // do nothing
            break;
          case TW_BUS_ERROR: // 0x00: Bus error due to illegal start or stop condition
            // reset internal hardware and release bus
            reg->twcr = (reg->twcr & TWCR_CMD_MASK) | BV(TWINT) | BV(TWSTO) | BV(TWEA);
            // set mode
            mode = IDLE;
            error = true;
            break;
        }
      }
      //****************************************************************************************
  };
//****************************************************************************************
}

#endif
