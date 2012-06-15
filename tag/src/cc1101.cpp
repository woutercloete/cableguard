#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
/****************************************************************************************/
#include "cc1101.h"
#include "spi.h"
#include "tag.h"
#include "crc.h"

#define RSSI_OFFSET 74
#define RSSI_VALID_DELAY_US    1300

#define CCA_RETRIES          4
#define BACKOFF_PERIOD_USECS 250

#define PKTSTATUS_CCA     _BV(4)
#define PKTSTATUS_CS      _BV(6)

#define STATE_IDLE    0
#define STATE_RX      1
#define STATE_TX      2
#define STATE_FSTXON  3
#define STATE_CALIB   4
#define STATE_SETTL   5
#define STATE_RXOVER  6
#define STATE_TXUNDER 7

#define WAIT_STATUS(status) while ( ((cmdStrobe(CC1101_SNOP)>>4) & 0x7) != status) ;

/******************************************************************************
 * PATABLE
 *******************************************************************************/
const u08 paTable[8] = { 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0 }; //7dbi power - 24mA

/****************************************************************************************/
static CC1101* pCC1101;
/****************************************************************************************/

/*****************************************************************************
 * INIT CC1101
 ****************************************************************************/
void CC1101::init(void) {
  pCC1101 = this;
  // Interrupts for GD0 - hooked up to PINE4
  disableIRQ_GDO0();
  spi.init();
  reset();
  setupRegs();
  writeBurstReg(CC1101_PATABLE, (u08*) paTable, 8);
  writeReg(CC1101_IOCFG0, CC1100_GDOx_SYNC_WORD);
#ifdef CC1101_INTERRUPT_DRIVEN
  enableIRQ_GDO0();
#endif
  writeReg(CC1101_IOCFG0, 0x09);

}

/*****************************************************************************
 * rssi_valid_wait
 ****************************************************************************/
void CC1101::goIdle(void) {
  switch (((cmdStrobe(CC1101_SNOP) >> 4) & 0x7)) {
    case STATE_RXOVER:
      cmdStrobe(CC1101_SRX);
      break;
    case STATE_TXUNDER:
      cmdStrobe(CC1101_STX);
      break;
    default:
      cmdStrobe(CC1101_SIDLE);
      break;
  }
  WAIT_STATUS(STATE_IDLE);
}

/*****************************************************************************
 * rssi_valid_wait
 ****************************************************************************/
void CC1101::rssiValidWait(void) {
  u08 delay;
  u08 stat;
  delay = 0;
  do {
    stat = readReg(CC1101_PKTSTATUS, CC1101_STATUS_REGISTER);
    if (stat & 0x50) {
      break;
    }
    _delay_us(10);
    delay++;
  } while (delay < 200);
}

bool CC1101::sendData1(u08 *data, u08 len) {
  setRxState();
  // Check that the RX state has been entered
  while ((readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1F) != 0x0D) {
    _delay_us(500);
  }
  cmdStrobe(CC1101_SFTX);
  // Set data len at the first position of the TX FIFO
  writeReg(CC1101_TXFIFO, len);
  // Write data u08o the TX FIFO
  writeBurstReg(CC1101_TXFIFO, data, len);
  // CCA enabled: will enter TX state only if the channel is clear
  cmdStrobe(CC1101_STX);
  // Check that TX state is being entered (state = RXTX_SETTLING)
  if ((readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1F) != 0x15) {
    // TODO CANNOT RETURN HERE - will reload the TX FIFO
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
  if ((readReg(CC1101_TXBYTES, CC1101_STATUS_REGISTER) & 0x7F) == 0) {
    return true;
  }

  return false;
}

/*****************************************************************************
 SendPacket
 The CC1101 expects a certain number of bytes in each packet
 given by the value in the PKTLEN register and transmits them when a
 TX strobe is issued. However, the expected number of bytes must be available in the
 TX FIFO or an underflow will occur
 *****************************************************************************/
void CC1101::transmit(u08 *data, u08 len, etxType txType) {
  u08 cca, cca_cnt, mode;
  cca = 0;
  cca_cnt = 0;
  mode = 0;
  u16 crc;
  /* Turn off receiver. We can ignore/drop incoming packets during transmit. */
  //RxModeOff();
  /* ==============================================================================
   FORCED TRANSMIT
   ================================================================================*/
  switch (txType) {
    case eCC1100_TX_TYPE_FORCED:
      crc = crc16(data, len, 0xCC);
      wait_GDO0_low()
      ;
      writeReg(CC1101_TXFIFO, len + 2); // Set data len at the first position of the TX FIFO +2 for crc's
      writeBurstReg(CC1101_TXFIFO, data, len); // Write packet data
      writeReg(CC1101_TXFIFO, (crc >> 8) & 0xFF);
      writeReg(CC1101_TXFIFO, (crc) & 0xFF);
      // Start Transmission
      cmdStrobe(CC1101_STX);
      wait_GDO0_low()
      ;
      cca_cnt = 0;
      // TX to IDle with calibration - 725 us
      do {
        mode = readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1F;
        cca_cnt++;
        _delay_us(10);
      } while ((mode != MARC_STATE_IDLE));
      break;

     case eCC1100_TX_TYPE_CCA:
      // url:http://e2e.ti.com/support/low_power_rf/w/design_notes/implementation-of-cca-in-an-application-cc1100-cc1101-cc1110-cc1111-cc2500-cc2510-cc2511.aspx
      // http://e2e.ti.com/support/low_power_rf/f/155/t/169134.aspx
      //The current CCA state is viewable on GD0
      //================= Send Packet to FIFO ==================
      crc = crc16(data, len, 0xCC);
      writeReg(CC1101_TXFIFO, len + 2);
      writeBurstReg(CC1101_TXFIFO, data, len);
      writeReg(CC1101_TXFIFO, (crc >> 8) & 0xFF);
      writeReg(CC1101_TXFIFO, (crc) & 0xFF);

      // Seed the randomizer
      srand(crc);
      cca_cnt = 0;
      while (1) {
        // ================= SET RX MODE==========================
        cmdStrobe(CC1101_SRX);
        wait_MARCSTATE(MARC_STATE_RX);
        // Give time for the RSSI levels to be sampled
        _delay_us(500);
        //rssiValidWait();
        // ================= SET TX MODE==========================
        cmdStrobe(CC1101_STX);
        // Datasheet RX->TX switch (~30us).
        _delay_us(50);
        mode = readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1F;
        //if(mode == MARC_STATE_TX || cca_cnt > 5 || BIT_IS_LO(READ_GD0,PIN_GD0)){
        if (mode == MARC_STATE_TX ) {
          break;
        }
        if (cca_cnt > 10) {
          goIdle();
          break;
        }
        cca_cnt++;
        cca = rand() & 0xFF;
        _delay_us(cca);
      }
      // TX to IDle with calibration - 725 us
      wait_MARCSTATE(MARC_STATE_IDLE);
      break;
  }
}

void CC1101::wait_PKTSTATUS(u08 status) {
  while (1) {
    if ((readReg(CC1101_PKTSTATUS, CC1101_STATUS_REGISTER) & 0x1F) == status) {
      break;
    }
    _delay_us(10);
  }
}

void CC1101::wait_MARCSTATE(u08 mode) {
  while (1) {
    if ((readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1F) == mode) {
      break;
    }
    _delay_us(10);
  }
}

/*****************************************************************************
 * Read data packet from RX FIFO
 ****************************************************************************/
void CC1101::ReceivePacket(void) {
  u08 len;
  u08 rxbytes;
  u08 pcktStatus;
  sRadioPacket packet;

  pcktStatus = readReg(CC1101_PKTSTATUS, CC1101_STATUS_REGISTER);
  rxbytes = readReg(CC1101_RXBYTES, CC1101_STATUS_REGISTER);
  if ((rxbytes > 8) && (pcktStatus & 0xA0)) { // chck GD0
    // Only length of payload
    len = readReg(CC1101_RXFIFO, CC1101_CONFIG_REGISTER);
    packet.size = len;
    if (len > 15) {
      cmdStrobe(CC1101_SFRX); // Flush RX FIFO - > OVERLFOW
      setRxState();
      tagreceived = false;
      return;
    }
    readBurstReg((u08*) &packet.tag, CC1101_RXFIFO, sizeof(TAG::sRfTag));
    packet.rssi = readReg(CC1101_RXFIFO, CC1101_CONFIG_REGISTER);
    packet.rssi = ((packet.rssi >> 1) + 74);
    packet.lqi = readReg(CC1101_RXFIFO, CC1101_CONFIG_REGISTER);
    packet.crc_ok = (readReg(CC1101_PKTSTATUS, CC1101_STATUS_REGISTER) & _BV(7));
    tagreceived = true;
#ifdef FIFO_ENABLED
    rxFifo.add(&packet, 1);
#endif
  }
// Enter back  RX state
  setRxState();
}

/*****************************************************************************
 * Reset CC1101
 ****************************************************************************/
void CC1101::reset(void) {
  nCS();
  _delay_us(5);
  CS();
  _delay_us(10);
  nCS();
  _delay_us(41);
  CS();

  MISO_WAIT();

  spi.send(CC1101_SRES); // Send reset command strobe
  MISO_WAIT();
  nCS();

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
  cmdStrobe(CC1101_SIDLE);
  cmdStrobe(CC1101_SPWD);
}
/******************************************************************************
 Wakeup from power down State
 ******************************************************************************/
void CC1101::wakeUp(void) {
  CS();
  MISO_WAIT(); /* wait for MISO to go high indicating the oscillator is stable */
  nCS();

  /*****************************************************************************/
  writeReg(CC1101_FSTEST, CC1101_DEFVAL_FSTEST);
  writeReg(CC1101_PTEST, CC1101_DEFVAL_PTEST);
  writeReg(CC1101_AGCTEST, CC1101_DEFVAL_AGCTEST);
  /*****************************************************************************/
  writeReg(CC1101_TEST2, CC1101_DEFVAL_TEST2);
  writeReg(CC1101_TEST1, CC1101_DEFVAL_TEST1);
  writeReg(CC1101_TEST0, CC1101_DEFVAL_TEST0);
  goIdle();
}
/******************************************************************************
 Write single register u08o the CC1101 IC via SPI
 ******************************************************************************/
void CC1101::writeReg(u08 regAddr, u08 value) {
  CS();
  MISO_WAIT();
  spi.send(regAddr);
  spi.send(value);
  nCS();
}
/******************************************************************************
 Write multiple registers u08o the CC1101 IC via SPI
 ******************************************************************************/
void CC1101::writeBurstReg(u08 regAddr, u08* buffer, u08 len) {
  CS();
  MISO_WAIT();
  spi.send((regAddr | WRITE_BURST));
  for (u08 i = 0; i < len; i++) {
    spi.send(buffer[i]);
  }
  nCS();
}
/******************************************************************************
 Send command strobe to the CC1101 IC via SPI
 ******************************************************************************/
u08 CC1101::cmdStrobe(u08 cmd) {
  u08 ret;
  CS();
  MISO_WAIT();
  ret = spi.send(cmd);
  nCS();
  return ret;
}
/******************************************************************************
 Read CC1101 register via SPI
 'regAddr'  Register address
 'regType'  Type of register: CC1101_CONFIG_REGISTER or CC1101_STATUS_REGISTER
 ******************************************************************************/
u08 CC1101::readReg(u08 regAddr, u08 regType) {
  u08 val;

  CS();
  MISO_WAIT();
  spi.send((regAddr | regType));
  val = spi.send(0x00);
  nCS();

  return val;
}
/*****************************************************************************
 Read a len of burst data from CC1101 via SPI u08o buffer
 *****************************************************************************/
void CC1101::readBurstReg(u08 * buffer, u08 regAddr, u08 len) {
  u08 addr, i;

  addr = regAddr | READ_BURST;
  CS();
  MISO_WAIT();
  spi.send(addr);
  for (i = 0; i < len; i++) {
    buffer[i] = spi.send(0x00);
  }
  nCS();
}

#if 0
u08 CC1101::Transmit(u08 *data, u08 len, etxType txType) {
  u08 ccaRetries;
  u08 mode;
  u08 returnValue = CC1100_TX_RESULT_SUCCESS;

  /* Turn off receiver. We can ignore/drop incoming packets during transmit. */
  RxModeOff();

  /* ==============================================================================
   *    Write packet to transmit FIFO
   ================================================================================*/
  writeReg(CC1101_TXFIFO, len);
  writeBurstReg(CC1101_TXFIFO, data, len);

  /* ------------------------------------------------------------------
   *    Immediate transmit
   *   ---------------------
   */
  if (txType == CC1100_TX_TYPE_FORCED) {
    //enableIRQ_GDO0();
    //Issue the TX strobe.
    cmdStrobe(CC1101_STX);
    //Wait for transmit to complete ISR will set GDO var low
    while (BIT_IS_HI(READ_GD0,PIN_GD0))
    ;
  } else {
    /* ------------------------------------------------------------------
     *    CCA transmit
     *   ---------------
     */
    ccaRetries = CCA_RETRIES;
    /* For CCA algorithm, we need to know the transition from the RX state to
     * the TX state. There is no need for SYNC signal in this logic. So we
     * can re-configure the GDO_0 output from the radio to be CCA SIGNAL
     * Since both SYNC and CCA are used as falling edge interrupts, we
     * don't need to reconfigure the MCU input.
     */
    writeReg(CC1101_IOCFG0, 0x09);
    /* ===============================================================================
     *    Main Loop
     * ===============================================================================
     */
    for (;;) {
      /* Radio must be in RX mode for CCA to happen.
       * Otherwise it will transmit without CCA happening.
       */
      disableIRQ_GDO0();
      cmdStrobe(CC1101_SRX);
      /* wait for the rssi to be valid. */
      rssiValidWait();
      /* send strobe to initiate transmit */
      cmdStrobe(CC1101_STX);

      if (pCC1101->gd0 == 0) {
        /* ------------------------------------------------------------------
         *    Clear Channel Assessment passed.
         *   ----------------------------------
         */
        /* Clear the PA_PD int flag */
        pCC1101->gd0 = 1;

        /* PA_PD signal stays LOW while in TX state and goes back to HIGH when
         * the radio transitions to RX state.
         */
        /* wait for transmit to complete */wait_GDO0_high(); // Wait until GDO0 line goes high
        break;
      } else {
        /* ------------------------------------------------------------------
         *    Clear Channel Assessment failed.
         *   ----------------------------------
         */
        /* Turn off radio and save some power during backoff */
        goIdle();
        /* flush the receive FIFO of any residual data */
        cmdStrobe(CC1101_SFRX);
        /* Retry ? */
        if (ccaRetries != 0) {
          /* delay for a random number of backoffs */
          //rndDelay();
          /* decrement CCA retries before loop continues */
          ccaRetries--;
        } else /* No CCA retries are left, abort */
        {
          /* set return value for failed transmit and break */
          returnValue = CC1100_TX_RESULT_FAILED;
          break;
        }
      } /* CCA Failed */
    } /* CCA loop */
  }/* txType is CCA */

  /* Done with TX. Clean up time... */
  /* Radio is already in IDLE state */

  /*
   * Flush the transmit FIFO.  It must be flushed so that
   * the next transmit can start with a clean slate.
   */
  cmdStrobe(CC1101_SFTX);
  /* Restore GDO_0 to be SYNC signal */
  writeReg(CC1101_IOCFG0, CC1100_GDOx_SYNC_WORD);

  return (returnValue);
}
#endif

ISR(PCINT0_vect) {
  if (BIT_IS_LO(READ_GD0,PIN_GD0)) {
    pCC1101->gd0 = 0;
  }
}

