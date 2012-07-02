/*! \file i2c.c \brief I2C interface using AVR Two-Wire Interface (TWI) hardware. */
//*****************************************************************************
//
// File Name	: 'i2c.c'
// Title		: I2C interface using AVR Two-Wire Interface (TWI) hardware
// Author		: Pascal Stang - Copyright (C) 2002-2003
// Created		: 2002.06.25
// Revised		: 2003.03.02
// Version		: 0.9
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************
#include <avr/io.h>
#include <avr/interrupt.h>
#include "common.h"
#include "CI2c.h"

// Standard I2C bit rates are:
// 100KHz for slow speed
// 400KHz for high speed

using namespace NCI2C;

#undef I2C_DEBUG

static CI2C* devices[4];

CI2C::CI2C(void) {
  // set pull-up resistors on I2C bus pins
  // TODO: should #ifdef these
/*
	BIT_SET_LO(DDRC, 0); // i2c SCL on ATmega163,323,16,32,etc
  BIT_SET_LO(DDRC, 1); // i2c SDA on ATmega163,323,16,32,etc
  BIT_SET_HI(PORTC, 0); // i2c SCL on ATmega163,323,16,32,etc
  BIT_SET_HI(PORTC, 1); // i2c SDA on ATmega163,323,16,32,etc
*/
#warning I2C ONLY FOR ATmega1280
  BIT_SET_LO(DDRD, 0);	// i2c SCL on ATmega128,64
  BIT_SET_LO(DDRD, 1);	// i2c SDA on ATmega128,64
  BIT_SET_HI(PORTD, 0);	// i2c SCL on ATmega128,64
  BIT_SET_HI(PORTD, 1);	// i2c SDA on ATmega128,64

  // clear SlaveReceive and SlaveTransmit handler to null
  slaveReceive = 0;
  slaveTransmit = 0;
  // set i2c bit rate to 400KHz
  setBitrate(400);
  // enable TWI (two-wire interface)
  BIT_SET_HI(TWCR, TWEN);
  // set state
  state = IDLE;

  // enable TWI interrupt and slave address ACK
  BIT_SET_HI(TWCR, TWIE);
  BIT_SET_HI(TWCR, TWEA);
  //outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
  devices[0] = this;
}

void CI2C::setBitrate(u16 bitrateKHz) {
  u08 bitrate_div;
  // set i2c bitrate
  // SCL freq = F_CPU/(16+2*TWBR))
#ifdef TWPS0
  // for processors with additional bitrate division (mega128)
  // SCL freq = F_CPU/(16+2*TWBR*4^TWPS)
  // set TWPS to zero
  BIT_SET_LO(TWSR, TWPS0);
  BIT_SET_LO(TWSR, TWPS1);
#endif
  // calculate bitrate division
  bitrate_div = ((F_CPU/1000l) / bitrateKHz);
  if (bitrate_div >= 16)
    bitrate_div = (bitrate_div - 16) / 2;
  TWBR = bitrate_div;
}

void CI2C::setLocalDeviceAddr(u08 deviceAddr, u08 genCallEn) {
  // set local device address (used in slave mode only)
  TWAR = ((deviceAddr & 0xFE) | (genCallEn ? 1 : 0));
}

void CI2C::setSlaveReceiveHandler(void(*slaveRx_func)(u08 receiveDataLength,
    u08* recieveData)) {
  slaveReceive = slaveRx_func;
}

void CI2C::setSlaveTransmitHandler(u08(*slaveTx_func)(
    u08 transmitDataLengthMax, u08* transmitData)) {
  slaveTransmit = slaveTx_func;
}

void CI2C::sendStart(void) {
  // send start condition
  //outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTA));
  TWCR = (TWCR & TWCR_CMD_MASK) | BV(TWINT) | BV(TWSTA);
}

void CI2C::sendStop(void) {
  // transmit stop condition
  // leave with TWEA on for slave receiving
  outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA)|BV(TWSTO));
}

void CI2C::waitForComplete(void) {
  // wait for i2c interface to complete operation
  while (!(inb(TWCR)& BV(TWINT)) );
}

void CI2C::sendByte(u08 data) {
  // save data to the TWDR
  outb(TWDR, data);
  // begin send
  outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
}

void CI2C::receiveByte(u08 ackFlag) {
  // begin receive over i2c
  if (ackFlag) {
    // ackFlag = TRUE: ACK the recevied data
    outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
  } else {
    // ackFlag = FALSE: NACK the recevied data
    outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
  }
}

u08 CI2C::getReceivedByte(void) {
  // retieve received data byte from i2c TWDR
  return (inb(TWDR));
}

u08 CI2C::getStatus(void) {
  // retieve current i2c status from i2c TWSR
  return (inb(TWSR));
}

void CI2C::masterSend(u08 deviceAddr, u08 length, u08* data) {
  u08 i;
  // wait for interface to be ready
  while (state)
    ;
  // set state
  state = MASTER_TX;
  // save data
  deviceAddrRW = (deviceAddr & 0xFE); // RW cleared: write operation
  for (i = 0; i < length; i++)
    sendData[i] = *data++;
  sendDataIndex = 0;
  sendDataLength = length;
  // send start condition
  sendStart();
}

void CI2C::masterReceive(u08 deviceAddr, u08 length, u08* data) {
  u08 i;
  // wait for interface to be ready
  while (state)
    ;
  // set state
  state = MASTER_RX;
  // save data
  deviceAddrRW = (deviceAddr | 0x01); // RW set: read operation
  receiveDataIndex = 0;
  receiveDataLength = length;
  // send start condition
  sendStart();
  // wait for data
  while (state)
    ;
  // return data
  for (i = 0; i < length; i++)
    *data++ = receiveData[i];
}
#ifdef NO_INTERRUPTS
u08 CI2C::masterSendNI(u08 deviceAddr, u08 length, u08* data)
{
  u08 retval = I2C_OK;

  // disable TWI interrupt
  BIT_SET_LO(TWCR, TWIE);

  // send start condition
  sendStart();
  waitForComplete();

  // send device address with write
  sendByte( deviceAddr & 0xFE );
  waitForComplete();

  // check if device is present and live
  if( inb(TWSR) == TW_MT_SLA_ACK)
  {
    // send data
    while(length)
    {
      sendByte( *data++ );
      waitForComplete();
      length--;
    }
  }
  else
  {
    // device did not ACK it's address,
    // data will not be transferred
    // return error
    retval = I2C_ERROR_NODEV;
  }

  // transmit stop condition
  // leave with TWEA on for slave receiving
  sendStop();
  while( !(inb(TWCR) & BV(TWSTO)) );

  // enable TWI interrupt
  BIT_SET_HI(TWCR, TWIE);

  return retval;
}

u08 CI2C::masterReceiveNI(u08 deviceAddr, u08 length, u08 *data)
{
  u08 retval = I2C_OK;

  // disable TWI interrupt
  BIT_SET_LO(TWCR, TWIE);

  // send start condition
  sendStart();
  waitForComplete();

  // send device address with read
  sendByte( deviceAddr | 0x01 );
  waitForComplete();

  // check if device is present and live
  if( inb(TWSR) == TW_MR_SLA_ACK)
  {
    // accept receive data and ack it
    while(length> 1)
    {
      receiveByte(TRUE);
      waitForComplete();
      *data++ = getReceivedByte();
      // decrement length
      length--;
    }

    // accept receive data and nack it (last-byte signal)
    receiveByte(FALSE);
    waitForComplete();
    *data++ = getReceivedByte();
  }
  else
  {
    // device did not ACK it's address,
    // data will not be transferred
    // return error
    retval = ERROR_NODEV;
  }

  // transmit stop condition
  // leave with TWEA on for slave receiving
  sendStop();

  // enable TWI interrupt
  BIT_SET_HI(TWCR, TWIE);

  return retval;
}
#endif

void CI2C::service() {
  // read status bits
  u08 status = inb(TWSR) & TWSR_STATUS_MASK;

  switch (status) {
    // Master General
    case TW_START: // 0x08: Sent start condition
    case TW_REP_START: // 0x10: Sent repeated start condition
#ifdef I2C_DEBUG
      ////rprintfInit(uart1AddToTxBuffer);
      //PRINTF("I2C: M->START\r\n");
      //rprintfInit(uart1SendByte);
#endif
      // send device address
      sendByte(deviceAddrRW);
      break;

      // Master Transmitter & Receiver status codes
    case TW_MT_SLA_ACK: // 0x18: Slave address acknowledged
    case TW_MT_DATA_ACK: // 0x28: Data acknowledged
#ifdef I2C_DEBUG
      //rprintfInit(uart1AddToTxBuffer);
      //PRINTF("I2C: MT->SLA_ACK or DATA_ACK\r\n");
      //rprintfInit(uart1SendByte);
#endif
      if (sendDataIndex < sendDataLength) {
        // send data
        sendByte(sendData[sendDataIndex++]);
      } else {
        // transmit stop condition, enable SLA ACK
        sendStop();
        // set state
        state = IDLE;
      }
      break;
    case TW_MR_DATA_NACK: // 0x58: Data received, NACK reply issued
#ifdef I2C_DEBUG
      //rprintfInit(uart1AddToTxBuffer);
      //PRINTF("I2C: MR->DATA_NACK\r\n");
      //rprintfInit(uart1SendByte);
#endif
      // store final received data byte
      receiveData[receiveDataIndex++] = inb(TWDR);
      // continue to transmit STOP condition
    case TW_MR_SLA_NACK: // 0x48: Slave address not acknowledged
    case TW_MT_SLA_NACK: // 0x20: Slave address not acknowledged
    case TW_MT_DATA_NACK: // 0x30: Data not acknowledged
#ifdef I2C_DEBUG
      //rprintfInit(uart1AddToTxBuffer);
      //PRINTF("I2C: MTR->SLA_NACK or MT->DATA_NACK\r\n");
      //rprintfInit(uart1SendByte);
#endif
      // transmit stop condition, enable SLA ACK
      devices[0]->sendStop();
      // set state
      state = IDLE;
      break;
    case TW_MT_ARB_LOST: // 0x38: Bus arbitration lost
      //case TW_MR_ARB_LOST:        // 0x38: Bus arbitration lost
#ifdef I2C_DEBUG
      //rprintfInit(uart1AddToTxBuffer);
      //PRINTF("I2C: MT->ARB_LOST\r\n");
      //rprintfInit(uart1SendByte);
#endif
      // release bus
      outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
      // set state
      state = IDLE;
      // release bus and transmit start when bus is free
      //outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTA));
      break;
    case TW_MR_DATA_ACK: // 0x50: Data acknowledged
#ifdef I2C_DEBUG
      //rprintfInit(uart1AddToTxBuffer);
      //PRINTF("I2C: MR->DATA_ACK\r\n");
      //rprintfInit(uart1SendByte);
#endif
      // store received data byte
      receiveData[receiveDataIndex++] = inb(TWDR);
      // fall-through to see if more bytes will be received
    case TW_MR_SLA_ACK: // 0x40: Slave address acknowledged
#ifdef I2C_DEBUG
      //rprintfInit(uart1AddToTxBuffer);
      //PRINTF("I2C: MR->SLA_ACK\r\n");
      //rprintfInit(uart1SendByte);
#endif
      if (receiveDataIndex < (receiveDataLength - 1))
        // data byte will be received, reply with ACK (more bytes in transfer)
        receiveByte(TRUE);
      else
        // data byte will be received, reply with NACK (final byte in transfer)
        receiveByte(FALSE);
      break;

      // Slave Receiver status codes
    case TW_SR_SLA_ACK: // 0x60: own SLA+W has been received, ACK has been returned
    case TW_SR_ARB_LOST_SLA_ACK: // 0x68: own SLA+W has been received, ACK has been returned
    case TW_SR_GCALL_ACK: // 0x70:     GCA+W has been received, ACK has been returned
    case TW_SR_ARB_LOST_GCALL_ACK: // 0x78:     GCA+W has been received, ACK has been returned
#ifdef I2C_DEBUG
      //rprintfInit(uart1AddToTxBuffer);
      //PRINTF("I2C: SR->SLA_ACK\r\n");
      //rprintfInit(uart1SendByte);
#endif
      // we are being addressed as slave for writing (data will be received from master)
      // set state
      state = SLAVE_RX;
      // prepare buffer
      receiveDataIndex = 0;
      // receive data byte and return ACK
      outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
      break;
    case TW_SR_DATA_ACK: // 0x80: data byte has been received, ACK has been returned
    case TW_SR_GCALL_DATA_ACK: // 0x90: data byte has been received, ACK has been returned
#ifdef I2C_DEBUG
      //rprintfInit(uart1AddToTxBuffer);
      //PRINTF("I2C: SR->DATA_ACK\r\n");
      //rprintfInit(uart1SendByte);
#endif
      // get previously received data byte
      receiveData[receiveDataIndex++] = inb(TWDR);
      // check receive buffer status
      if (receiveDataIndex < I2C_RECEIVE_DATA_BUFFER_SIZE) {
        // receive data byte and return ACK
        receiveByte(TRUE);
        //outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
      } else {
        // receive data byte and return NACK
        receiveByte(FALSE);
        //outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
      }
      break;
    case TW_SR_DATA_NACK: // 0x88: data byte has been received, NACK has been returned
    case TW_SR_GCALL_DATA_NACK: // 0x98: data byte has been received, NACK has been returned
#ifdef I2C_DEBUG
      //rprintfInit(uart1AddToTxBuffer);
      //PRINTF("I2C: SR->DATA_NACK\r\n");
      //rprintfInit(uart1SendByte);
#endif
      // receive data byte and return NACK
      receiveByte(FALSE);
      //outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
      break;
    case TW_SR_STOP: // 0xA0: STOP or REPEATED START has been received while addressed as slave
#ifdef I2C_DEBUG
      //rprintfInit(uart1AddToTxBuffer);
      //PRINTF("I2C: SR->SR_STOP\r\n");
      //rprintfInit(uart1SendByte);
#endif
      // switch to SR mode with SLA ACK
      outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
      // i2c receive is complete, call CI2C::slaveReceive
      if (slaveReceive)
        slaveReceive(receiveDataIndex, receiveData);
      // set state
      state = IDLE;
      break;

      // Slave Transmitter
    case TW_ST_SLA_ACK: // 0xA8: own SLA+R has been received, ACK has been returned
    case TW_ST_ARB_LOST_SLA_ACK: // 0xB0:     GCA+R has been received, ACK has been returned
#ifdef I2C_DEBUG
      //rprintfInit(uart1AddToTxBuffer);
      //PRINTF("I2C: ST->SLA_ACK\r\n");
      //rprintfInit(uart1SendByte);
#endif
      // we are being addressed as slave for reading (data must be transmitted back to master)
      // set state
      state = SLAVE_TX;
      // request data from application
      if (slaveTransmit)
        sendDataLength = slaveTransmit(I2C_SEND_DATA_BUFFER_SIZE, sendData);
      // reset data index
      sendDataIndex = 0;
      // fall-through to transmit first data byte
    case TW_ST_DATA_ACK: // 0xB8: data byte has been transmitted, ACK has been received
#ifdef I2C_DEBUG
      //rprintfInit(uart1AddToTxBuffer);
      //PRINTF("I2C: ST->DATA_ACK\r\n");
      //rprintfInit(uart1SendByte);
#endif
      // transmit data byte
      outb(TWDR, sendData[sendDataIndex++]);
      if (sendDataIndex < sendDataLength)
        // expect ACK to data byte
        outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
      else
        // expect NACK to data byte
        outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
      break;
    case TW_ST_DATA_NACK: // 0xC0: data byte has been transmitted, NACK has been received
    case TW_ST_LAST_DATA: // 0xC8:
#ifdef I2C_DEBUG
      //rprintfInit(uart1AddToTxBuffer);
      //PRINTF("I2C: ST->DATA_NACK or LAST_DATA\r\n");
      //rprintfInit(uart1SendByte);
#endif
      // all done
      // switch to open slave
      outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
      // set state
      state = IDLE;
      break;

      // Misc
    case TW_NO_INFO: // 0xF8: No relevant state information
      // do nothing
#ifdef I2C_DEBUG
      //rprintfInit(uart1AddToTxBuffer);
      //PRINTF("I2C: NO_INFO\r\n");
      //rprintfInit(uart1SendByte);
#endif
      break;
    case TW_BUS_ERROR: // 0x00: Bus error due to illegal start or stop condition
#ifdef I2C_DEBUG
      //rprintfInit(uart1AddToTxBuffer);
      //PRINTF("I2C: BUS_ERROR\r\n");
      //rprintfInit(uart1SendByte);
#endif
      // reset internal hardware and release bus
      outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTO)|BV(TWEA));
      // set state
      state = IDLE;
      break;
  }
}

//! I2C (TWI) interrupt service routine
SIGNAL(SIG_2WIRE_SERIAL)
{
  devices[0]->service();
}

eState CI2C::getState(void) {
  return state;
}
