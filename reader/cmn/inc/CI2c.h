#ifndef CI2C_H
#define CI2C_H

#include "avrlibtypes.h"
#include "avrlibdefs.h"

namespace NCI2C {
  // TWSR values (not bits)
  // (taken from avr-libc twi.h - thank you Marek Michalkiewicz)
  // Master
#define TW_START					0x08
#define TW_REP_START				0x10
  // Master Transmitter
#define TW_MT_SLA_ACK				0x18
#define TW_MT_SLA_NACK				0x20
#define TW_MT_DATA_ACK				0x28
#define TW_MT_DATA_NACK				0x30
#define TW_MT_ARB_LOST				0x38
  // Master Receiver
#define TW_MR_ARB_LOST				0x38
#define TW_MR_SLA_ACK				0x40
#define TW_MR_SLA_NACK				0x48
#define TW_MR_DATA_ACK				0x50
#define TW_MR_DATA_NACK				0x58
  // Slave Transmitter
#define TW_ST_SLA_ACK				0xA8
#define TW_ST_ARB_LOST_SLA_ACK		0xB0
#define TW_ST_DATA_ACK				0xB8
#define TW_ST_DATA_NACK				0xC0
#define TW_ST_LAST_DATA				0xC8
  // Slave Receiver
#define TW_SR_SLA_ACK				0x60
#define TW_SR_ARB_LOST_SLA_ACK		0x68
#define TW_SR_GCALL_ACK				0x70
#define TW_SR_ARB_LOST_GCALL_ACK	0x78
#define TW_SR_DATA_ACK				0x80
#define TW_SR_DATA_NACK				0x88
#define TW_SR_GCALL_DATA_ACK		0x90
#define TW_SR_GCALL_DATA_NACK		0x98
#define TW_SR_STOP					0xA0
  // Misc
#define TW_NO_INFO					0xF8
#define TW_BUS_ERROR				0x00

  // defines and constants
#define TWCR_CMD_MASK		0x0F
#define TWSR_STATUS_MASK	0xF8

  // return values
#define OK				0x00
#define ERROR_NODEV		0x01

#define I2C_SEND_DATA_BUFFER_SIZE       0x10
#define I2C_RECEIVE_DATA_BUFFER_SIZE    0x10


  typedef enum {
    IDLE = 0,
    BUSY = 1,
    MASTER_TX = 2,
    MASTER_RX = 3,
    SLAVE_TX = 4,
    SLAVE_RX = 5
  } eState;

  class CI2C {
    public:

      CI2C(void);
      void setBitrate(u16 bitrateKHz);
      void setLocalDeviceAddr(u08 deviceAddr, u08 genCallEn);
      void setSlaveReceiveHandler(void(*i2cSlaveRx_func)(u08 receiveDataLength,
          u08* recieveData));
      void setSlaveTransmitHandler(u08(*i2cSlaveTx_func)(
          u08 transmitDataLengthMax, u08* transmitData));
      void masterSend(u08 deviceAddr, u08 length, u08 *data);
      void masterReceive(u08 deviceAddr, u08 length, u08* data);
      u08 masterSendNI(u08 deviceAddr, u08 length, u08* data);
      u08 masterReceiveNI(u08 deviceAddr, u08 length, u08 *data);
      eState getState(void);
      void service(void);

      volatile eState state;
      u08 sendData[I2C_SEND_DATA_BUFFER_SIZE];
      u08 sendDataIndex;
      u08 sendDataLength;
      u08 receiveData[I2C_RECEIVE_DATA_BUFFER_SIZE];
      u08 receiveDataIndex;
      u08 receiveDataLength;
      void (*slaveReceive)(u08 receiveDataLength, u08* recieveData);
      u08 (*slaveTransmit)(u08 transmitDataLengthMax, u08* transmitData);

    private:
      u08 deviceAddrRW;
      void sendStart(void);
      void sendStop(void);
      void waitForComplete(void);
      void sendByte(u08 data);
      void receiveByte(u08 ackFlag);
      u08 getReceivedByte(void);
      u08 getStatus(void);
  };
}
#endif
