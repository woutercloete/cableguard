#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

extern "C" {
#include "spi.h"

}
#include "cc1101.h"
#include "rfstudio.h"
#include "crc.h"

#define SPI_SS   0
#define SPI_MOSI 2
#define SPI_MISO 3
#define SPI_SCK  1

#define BIT_GDO0 2

#define CS()                      cbi(PORTB, SPI_SS)
#define nCS()                     sbi(PORTB, SPI_SS)
#define MISO_WAIT()               LOOP_UNTIL_BIT_IS_LO(PINB,SPI_MISO); // Wait until SPI MISO line goes low
#define wait_GDO0_high()          LOOP_UNTIL_BIT_IS_HI(PINF,BIT_GDO0);// Wait until GDO0 line goes high
#define wait_GDO0_low()           LOOP_UNTIL_BIT_IS_LO(PINF,BIT_GDO0);// Wait until GDO0 line goes low
#define readConfigReg(regAddr)    readReg(regAddr, CC1101_CONFIG_REGISTER)
#define readStatusReg(regAddr)    readReg(regAddr, CC1101_STATUS_REGISTER)

/******************************************************************************
 * PATABLE
 *******************************************************************************/
const u08 paTable[8] = { 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60 };
using namespace C1101;

static CC1101* pCC1101;
/*****************************************************************************
 * Reset CC1101
 ****************************************************************************/
void CC1101::init(void) {
  pCC1101 = this;
  // Interrupts for GD0 - hooked up to PINE4
  disableIRQ_GDO0();
  //- Asserts when sync word has been sent / received, and de-asserts at the end of the packet
  EICRB = _BV(ISC71); //   The falling edge on INT7

  cbi(DDRE, PE7);
  // Input
  cbi(DDRF, PF2);
  //GD0 is an input
  cbi(PORTE, PE7);
  // No pullup

  //LED0 Port Direction
  sbi(DDRF, PF0);

  SPI0_Init();

  reset();
  setupRegs();

  writeBurstReg(CC1101_PATABLE, (u08*) paTable, 8);
  setDevAddress(0x00);
}

/*****************************************************************************
 * Send data packet via RF
 *****************************************************************************/
bool CC1101::sendData(u08 *data, u08 len) {
  setRxState();
  // Check that the RX state has been entered
  while ((readStatusReg(CC1101_MARCSTATE) & 0x1F) != 0x0D) {
    _delay_us(500);
  }
  // Set data len at the first position of the TX FIFO
  writeReg(CC1101_TXFIFO, len);
  // Write data u08o the TX FIFO
  writeBurstReg(CC1101_TXFIFO, data, len);
  // CCA enabled: will enter TX state only if the channel is clear
  cmdStrobe(CC1101_STX);
  // Check that TX state is being entered (state = RXTX_SETTLING)
  if ((readStatusReg(CC1101_MARCSTATE) & 0x1F) != 0x15) {
    return false;
  }
  // Wait for the sync word to be transmitted
  wait_GDO0_high();
  // Wait until the end of the packet transmission
  wait_GDO0_low();
  // Flush TX FIFO. Don't uncomment
  // cmdStrobe(CC1101_SFTX);
  // Enter back u08o RX state
  setRxState();
  // Check that the TX FIFO is empty
  if ((readStatusReg(CC1101_TXBYTES) & 0x7F) == 0) {
    return true;
  }
  return false;
}

/*****************************************************************************
 * TODO HELP BIETJIE HIER
 *****************************************************************************/
void CC1101::SendPacket(void) {
  setRxState();
  sRadioPacket packet;
  // Check that the RX state has been entered
  while ((readStatusReg(CC1101_MARCSTATE) & 0x1F) != 0x0D) {
    _delay_us(500);
  }

  if (txFifo.remove(&packet, 1)) {
    writeBurstReg(CC1101_TXFIFO, (u08*) &packet, packet.size);
    cmdStrobe(CC1101_SIDLE);
    cmdStrobe(CC1101_STX);
  }
  wait_GDO0_high(); // TODO - PUT IN ISR Wait for the sync word to be transmitted
  wait_GDO0_low(); // TODO - PUT IN ISR Wait until the end of the packet transmission
  cmdStrobe(CC1101_SFTX); // Flush TX FIFO. Don't uncomment
  setRxState(); // Enter back  RX state
  // Check that the TX FIFO is not empty - put packet aback onto queue
  if ((readStatusReg(CC1101_TXBYTES) & 0x7F)) {
    txFifo.add(&packet, 1);
  }
}

/*****************************************************************************
 * Read data packet from RX FIFO
 ****************************************************************************/
void CC1101::ReceivePacket(void) {
  u08 len;
  u16 crc;
  u16 pcktCrc;
  u08 rxbytes;
  u08 pcktStatus;
  sRadioPacket packet;
  u08 testBuf[16];

  isrCnt++;
  pcktStatus = readStatusReg(CC1101_PKTSTATUS);
  rxbytes = readStatusReg(CC1101_RXBYTES);
  if ((rxbytes > 4) && (pcktStatus & 0xA0)) { // chck GD0
    // Only length of payload
    wait_GDO0_low(); //Packet received in full
    len = readConfigReg(CC1101_RXFIFO);
    packet.size = len;
    if (len > 15) {
      cmdStrobe(CC1101_SIDLE);
      _delay_us(100);
      cmdStrobe(CC1101_SFRX); // Flush RX FIFO - > OVERLFOW
      tagreceived = false;
      goto exit;
    }
    readBurstReg(testBuf, CC1101_RXFIFO, len);
    pcktCrc = testBuf[len-2];
    pcktCrc = (pcktCrc << 8) + testBuf[len-1];
    crc = crc16(testBuf, len - 2, 0xCC);
    if (len != 0xb){
      len++;
      len--;
    }
    //readBurstReg((u08*) &packet.tag, CC1101_RXFIFO, sizeof(TAG::sRfTag));
    //wait_GDO0_low(); //Packet received in full

//    pcktCrc = readConfigReg(CC1101_RXFIFO);
//    pcktCrc = (pcktCrc << 8) + readConfigReg(CC1101_RXFIFO);
//    crc = crc16((u08*) &packet.tag, len - 2, 0xCC);
    if (crc == pcktCrc) {
      memcpy(&packet.tag, testBuf,sizeof(packet.tag));
      packet.rssi = convDB(readConfigReg(CC1101_RXFIFO));
      packet.lqi = readConfigReg(CC1101_RXFIFO);
      packet.crc_ok = (readStatusReg(CC1101_PKTSTATUS) & _BV(7));
      tagreceived = true;
      rxFifo.add(&packet, 1);
      rxCnt++;
      if (packet.tag.tagID == 0x00110001){
        rxCnt--;
        rxCnt++;
      }
    } else {
      cmdStrobe(CC1101_SFRX); // Flush RX FIFO
    }
  }
  exit: setRxState();
  /*
   while(1){
   marc = readStatusReg(CC1101_MARCSTATE) & 0x1F;
   _delay_us(20);
   if(marc==13){
   break;
   }
   }
   */

}

/*****************************************************************************
 * Reset CC1101
 ****************************************************************************/
void CC1101::reset(void) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    SPI0_MODE0();
    nCS();
    _delay_us(5);
    CS();
    _delay_us(10);
    nCS();
    _delay_us(41);
    CS();
    MISO_WAIT();
    SPI0_WriteReadByte(CC1101_SRES); // Send reset command strobe
    MISO_WAIT();
    nCS();
  }
}

/********************************************************************************
 * Configure CC1101 registers according to RF DESIGN STUDIO
 *******************************************************************************/
void CC1101::setupRegs(void) {
  /*****************************************************************************/
  writeReg(CC1101_IOCFG2, CC1101_DEFVAL_IOCFG2); //GDO2 Output Pin Configuration
  writeReg(CC1101_IOCFG1, CC1101_DEFVAL_IOCFG1); // GDO1 Output Pin Configuration
  writeReg(CC1101_IOCFG0, CC1101_DEFVAL_IOCFG0); // GDO0 Output Pin Configuration
  writeReg(CC1101_FIFOTHR, CC1101_DEFVAL_FIFOTHR); // RX FIFO and TX FIFO Thresholds

  /*****************************************************************************/
  writeReg(CC1101_SYNC1, CC1101_DEFVAL_SYNC1); // Sync Word, High Byte
  writeReg(CC1101_SYNC0, CC1101_DEFVAL_SYNC0); // Sync Word, Low Byte
  /*****************************************************************************/
  writeReg(CC1101_PKTLEN, CC1101_DEFVAL_PKTLEN);
  writeReg(CC1101_PKTCTRL1, CC1101_DEFVAL_PKTCTRL1);
  writeReg(CC1101_PKTCTRL0, CC1101_DEFVAL_PKTCTRL0);
  /*****************************************************************************/
  writeReg(CC1101_ADDR, CC1101_DEFVAL_ADDR); // Device Address
  writeReg(CC1101_CHANNR, CC1101_DEFVAL_CHANNR); // Channel Number
  /*****************************************************************************/
  writeReg(CC1101_FSCTRL1, CC1101_DEFVAL_FSCTRL1); //Frequency Synthesizer Control
  writeReg(CC1101_FSCTRL0, CC1101_DEFVAL_FSCTRL0);
  /*****************************************************************************/
  writeReg(CC1101_FREQ2, CC1101_DEFVAL_FREQ2); // Frequency Control
  writeReg(CC1101_FREQ1, CC1101_DEFVAL_FREQ1);
  writeReg(CC1101_FREQ0, CC1101_DEFVAL_FREQ0);
  /*****************************************************************************/
  writeReg(CC1101_MDMCFG4, CC1101_DEFVAL_MDMCFG4); // Modem Configuration
  writeReg(CC1101_MDMCFG3, CC1101_DEFVAL_MDMCFG3);
  writeReg(CC1101_MDMCFG2, CC1101_DEFVAL_MDMCFG2);
  writeReg(CC1101_MDMCFG1, CC1101_DEFVAL_MDMCFG1);
  writeReg(CC1101_MDMCFG0, CC1101_DEFVAL_MDMCFG0);
  /*****************************************************************************/
  writeReg(CC1101_DEVIATN, CC1101_DEFVAL_DEVIATN); // Modem Deviation Setting
  /*****************************************************************************/
  writeReg(CC1101_MCSM2, CC1101_DEFVAL_MCSM2); //Main Radio Control State Machine
  writeReg(CC1101_MCSM1, CC1101_DEFVAL_MCSM1);
  writeReg(CC1101_MCSM0, CC1101_DEFVAL_MCSM0);
  /*****************************************************************************/
  writeReg(CC1101_FOCCFG, CC1101_DEFVAL_FOCCFG);
  writeReg(CC1101_BSCFG, CC1101_DEFVAL_BSCFG);
  /*****************************************************************************/
  writeReg(CC1101_AGCCTRL2, CC1101_DEFVAL_AGCCTRL2);
  writeReg(CC1101_AGCCTRL1, CC1101_DEFVAL_AGCCTRL1);
  writeReg(CC1101_AGCCTRL0, CC1101_DEFVAL_AGCCTRL0);
  /*****************************************************************************/
  writeReg(CC1101_WOREVT1, CC1101_DEFVAL_WOREVT1);
  writeReg(CC1101_WOREVT0, CC1101_DEFVAL_WOREVT0);
  writeReg(CC1101_WORCTRL, CC1101_DEFVAL_WORCTRL);
  /*****************************************************************************/
  writeReg(CC1101_FREND1, CC1101_DEFVAL_FREND1);
  writeReg(CC1101_FREND0, CC1101_DEFVAL_FREND0);
  /*****************************************************************************/
  writeReg(CC1101_FSCAL3, CC1101_DEFVAL_FSCAL3);
  writeReg(CC1101_FSCAL2, CC1101_DEFVAL_FSCAL2);
  writeReg(CC1101_FSCAL1, CC1101_DEFVAL_FSCAL1);
  writeReg(CC1101_FSCAL0, CC1101_DEFVAL_FSCAL0);
  /*****************************************************************************/
  writeReg(CC1101_RCCTRL1, CC1101_DEFVAL_RCCTRL1);
  writeReg(CC1101_RCCTRL0, CC1101_DEFVAL_RCCTRL0);
  /*****************************************************************************/
  writeReg(CC1101_FSTEST, CC1101_DEFVAL_FSTEST);
  writeReg(CC1101_PTEST, CC1101_DEFVAL_PTEST);
  writeReg(CC1101_AGCTEST, CC1101_DEFVAL_AGCTEST);
  /*****************************************************************************/
  writeReg(CC1101_TEST2, CC1101_DEFVAL_TEST2);
  writeReg(CC1101_TEST1, CC1101_DEFVAL_TEST1);
  writeReg(CC1101_TEST0, CC1101_DEFVAL_TEST0);
}

/*****************************************************************************
 * 'sync' Synchronization word
 *****************************************************************************/
void CC1101::setSyncWord(u08 *sync) {
  if ((syncWord[0] != sync[0]) || (syncWord[1] != sync[1])) {
    writeReg(CC1101_SYNC1, sync[1]);
    writeReg(CC1101_SYNC0, sync[0]);
    memcpy(syncWord, sync, sizeof(syncWord));
  }
}

/*****************************************************************************
 * 'addr'	Device address
 *****************************************************************************/
void CC1101::setDevAddress(u08 addr) {
  if (devAddress != addr) {
    writeReg(CC1101_ADDR, addr);
    devAddress = addr;
  }
}
/*****************************************************************************
 * Set frequency channel
 * 'chnl'  Frequency channel
 * 'save' If TRUE, save parameter in EEPROM
 *****************************************************************************/
void CC1101::setChannel(u08 chnl) {
  if (channel != chnl) {
    writeReg(CC1101_CHANNR, chnl);
    channel = chnl;
  }
}
/*****************************************************************************
 * Put CC1101 u08o power-down state
 *****************************************************************************/
void CC1101::powerdown() {
  // If Coming from RX state, we need to enter the IDLE state first
  cmdStrobe(CC1101_SIDLE);
  // Enter Power-down state
  cmdStrobe(CC1101_SPWD);
}
/******************************************************************************
 Wakeup from power down State
 ******************************************************************************/
void CC1101::wakeUp(void) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    SPI0_MODE0();
    CS();
    MISO_WAIT();
    nCS();
  }
}
/******************************************************************************
 Write single register u08o the CC1101 IC via SPI
 ******************************************************************************/
void CC1101::writeReg(u08 regAddr, u08 value) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    SPI0_MODE0();
    CS();
    MISO_WAIT();
    SPI0_WriteReadByte(regAddr);
    SPI0_WriteReadByte(value);
    nCS();
  }
}
/******************************************************************************
 Write multiple registers u08o the CC1101 IC via SPI
 ******************************************************************************/
void CC1101::writeBurstReg(u08 regAddr, u08* buffer, u08 len) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    SPI0_MODE0();
    CS();
    MISO_WAIT();
    SPI0_WriteReadByte((regAddr | WRITE_BURST));
    for (u08 i = 0; i < len; i++) {
      SPI0_WriteReadByte(buffer[i]);
    }
    nCS();
  }
}
/******************************************************************************
 Send command strobe to the CC1101 IC via SPI
 ******************************************************************************/
void CC1101::cmdStrobe(u08 cmd) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    SPI0_MODE0();
    CS();
    MISO_WAIT();
    SPI0_WriteReadByte(cmd);
    nCS();
  }
}
/******************************************************************************
 Read CC1101 register via SPI
 'regAddr'  Register address
 'regType'  Type of register: CC1101_CONFIG_REGISTER or CC1101_STATUS_REGISTER
 ******************************************************************************/
u08 CC1101::readReg(u08 regAddr, u08 regType) {
  u08 val;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    SPI0_MODE0();
    CS();
    MISO_WAIT();
    SPI0_WriteReadByte((regAddr | regType));
    val = SPI0_WriteReadByte(0x00);
    nCS();
  }
  return val;
}
/*****************************************************************************
 Read a len of burst data from CC1101 via SPI u08o buffer
 *****************************************************************************/
void CC1101::readBurstReg(u08 * buffer, u08 regAddr, u08 len) {
  u08 addr, i;

  addr = regAddr | READ_BURST;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    SPI0_MODE0();
    CS();
    MISO_WAIT();
    SPI0_WriteReadByte(addr);
    for (i = 0; i < len; i++) {
      buffer[i] = SPI0_WriteReadByte(0x00);
    }
    nCS();
  }
}

ISR(INT7_vect)
{
  BIT_TOGGLE(PORTF, PF0);
  pCC1101->ReceivePacket();
}

