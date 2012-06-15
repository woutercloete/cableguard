#ifndef _CC1101_H
#define _CC1101_H

#include "avrlibdefs.h"
#include "avrlibtypes.h"
#include "common.h"
#include "spi.h"
#include "rfstudio.h"
#include "Tfifo.h"
#include "tag.h"

#define PORT_GD0  PORTB
#define READ_GD0  PINB
#define DDR_GD0   DDRB
#define PIN_GD0   6


#undef CC1101_INTERRUPT_DRIVEN
#undef FIFOS_ENABLED
#define RFID_TAG
#undef RFID_READER

/*****************************************************************************
 * FIFO SIZE
 *****************************************************************************/
#define BYTES_IN_RXFIFO 0x7F

/*****************************************************************************
 * Frequency channels
 *****************************************************************************/
#define NUMBER_OF_FCHANNELS      10

/*****************************************************************************
 * Type of transfers
 *****************************************************************************/
#define WRITE_BURST              0x40
#define READ_SINGLE              0x80
#define READ_BURST               0xC0

/*****************************************************************************
 * Type of register
 *****************************************************************************/
#define CC1101_CONFIG_REGISTER   READ_SINGLE
#define CC1101_STATUS_REGISTER   READ_BURST
/*****************************************************************************
 * TIME CONSTANTS
 *****************************************************************************/
#define CC1100_POWER_UP_DELAY_NS                40000
#define CC1100_POWER_UP_DELAY_US                40
#define CC1100_MANCAL_DELAY_NS                  721000
#define CC1100_MANCAL_DELAY_US                  721
#define CC1100_FS_WAKEUP_DELAY_NS               44200
#define CC1100_FS_WAKEUP_DELAY_US               45
#define CC1100_SETTLING_DELAY_NS                44200
#define CC1100_SETTLING_DELAY_US                44
#define CC1100_CALIBRATE_DELAY_NS               720600
#define CC1100_CALIBRATE_DELAY_US               720
#define CC1100_IDLE_NOCAL_DELAY_NS              100
#define CC1100_IDLE_NOCAL_DELAY_US              1
#define CC1100_TX_RX_DELAY_NS                   21500
#define CC1100_TX_RX_DELAY_US                   22
#define CC1100_RX_TX_DELAY_NS                   9600
#define CC1100_RX_TX_DELAY_US                   10

/*****************************************************************************
 * PATABLE & FIFO's
 *****************************************************************************/
#define CC1101_PATABLE           0x3E        // PATABLE address
#define CC1101_TXFIFO            0x3F        // TX FIFO address
#define CC1101_RXFIFO            0x3F        // RX FIFO address
/*****************************************************************************
 * Command strobes
 *****************************************************************************/
#define CC1101_SRES              0x30        // Reset CC1101 chip
#define CC1101_SFSTXON           0x31        // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1). If in RX (with CCA):
// Go to a wait state where only the synthesizer is running (for quick RX / TX turnaround).
#define CC1101_SXOFF             0x32        // Turn off crystal oscillator
#define CC1101_SCAL              0x33        // Calibrate frequency synthesizer and turn it off. SCAL can be strobed from IDLE mode without
// setting manual calibration mode (MCSM0.FS_AUTOCAL=0)
#define CC1101_SRX               0x34        // Enable RX. Perform calibration first if coming from IDLE and MCSM0.FS_AUTOCAL=1
#define CC1101_STX               0x35        // In IDLE state: Enable TX. Perform calibration first if MCSM0.FS_AUTOCAL=1.
// If in RX state and CCA is enabled: Only go to TX if channel is clear
#define CC1101_SIDLE             0x36        // Exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable
#define CC1101_SWOR              0x38        // Start automatic RX polling sequence (Wake-on-Radio) as described in Section 19.5 if
// WORCTRL.RC_PD=0
#define CC1101_SPWD              0x39        // Enter power down mode when CSn goes high
#define CC1101_SFRX              0x3A        // Flush the RX FIFO buffer. Only issue SFRX in IDLE or RXFIFO_OVERFLOW states
#define CC1101_SFTX              0x3B        // Flush the TX FIFO buffer. Only issue SFTX in IDLE or TXFIFO_UNDERFLOW states
#define CC1101_SWORRST           0x3C        // Reset real time clock to Event1 value
#define CC1101_SNOP              0x3D        // No operation. May be used to get access to the chip status byte
/*****************************************************************************
 * CC1101 configuration registers
 *****************************************************************************/
#define CC1101_IOCFG2            0x00        // GDO2 Output Pin Configuration
#define CC1101_IOCFG1            0x01        // GDO1 Output Pin Configuration
#define CC1101_IOCFG0            0x02        // GDO0 Output Pin Configuration
#define CC1101_FIFOTHR           0x03        // RX FIFO and TX FIFO Thresholds
#define CC1101_SYNC1             0x04        // Sync Word, High Byte
#define CC1101_SYNC0             0x05        // Sync Word, Low Byte
#define CC1101_PKTLEN            0x06        // Packet Length
#define CC1101_PKTCTRL1          0x07        // Packet Automation Control
#define CC1101_PKTCTRL0          0x08        // Packet Automation Control
#define CC1101_ADDR              0x09        // Device Address
#define CC1101_CHANNR            0x0A        // Channel Number
#define CC1101_FSCTRL1           0x0B        // Frequency Synthesizer Control
#define CC1101_FSCTRL0           0x0C        // Frequency Synthesizer Control
#define CC1101_FREQ2             0x0D        // Frequency Control Word, High Byte
#define CC1101_FREQ1             0x0E        // Frequency Control Word, Middle Byte
#define CC1101_FREQ0             0x0F        // Frequency Control Word, Low Byte
#define CC1101_MDMCFG4           0x10        // Modem Configuration
#define CC1101_MDMCFG3           0x11        // Modem Configuration
#define CC1101_MDMCFG2           0x12        // Modem Configuration
#define CC1101_MDMCFG1           0x13        // Modem Configuration
#define CC1101_MDMCFG0           0x14        // Modem Configuration
#define CC1101_DEVIATN           0x15        // Modem Deviation Setting
#define CC1101_MCSM2             0x16        // Main Radio Control State Machine Configuration
#define CC1101_MCSM1             0x17        // Main Radio Control State Machine Configuration
#define CC1101_MCSM0             0x18        // Main Radio Control State Machine Configuration
#define CC1101_FOCCFG            0x19        // Frequency Offset Compensation Configuration
#define CC1101_BSCFG             0x1A        // Bit Synchronization Configuration
#define CC1101_AGCCTRL2          0x1B        // AGC Control
#define CC1101_AGCCTRL1          0x1C        // AGC Control
#define CC1101_AGCCTRL0          0x1D        // AGC Control
#define CC1101_WOREVT1           0x1E        // High Byte Event0 Timeout
#define CC1101_WOREVT0           0x1F        // Low Byte Event0 Timeout
#define CC1101_WORCTRL           0x20        // Wake On Radio Control
#define CC1101_FREND1            0x21        // Front End RX Configuration
#define CC1101_FREND0            0x22        // Front End TX Configuration
#define CC1101_FSCAL3            0x23        // Frequency Synthesizer Calibration
#define CC1101_FSCAL2            0x24        // Frequency Synthesizer Calibration
#define CC1101_FSCAL1            0x25        // Frequency Synthesizer Calibration
#define CC1101_FSCAL0            0x26        // Frequency Synthesizer Calibration
#define CC1101_RCCTRL1           0x27        // RC Oscillator Configuration
#define CC1101_RCCTRL0           0x28        // RC Oscillator Configuration
#define CC1101_FSTEST            0x29        // Frequency Synthesizer Calibration Control
#define CC1101_PTEST             0x2A        // Production Test
#define CC1101_AGCTEST           0x2B        // AGC Test
#define CC1101_TEST2             0x2C        // Various Test Settings
#define CC1101_TEST1             0x2D        // Various Test Settings
#define CC1101_TEST0             0x2E        // Various Test Settings
/*****************************************************************************
 * Status registers
 *****************************************************************************/
#define CC1101_PARTNUM           0x30        // Chip ID
#define CC1101_VERSION           0x31        // Chip ID
#define CC1101_FREQEST           0x32        // Frequency Offset Estimate from Demodulator
#define CC1101_LQI               0x33        // Demodulator Estimate for Link Quality
#define CC1101_RSSI              0x34        // Received Signal Strength Indication
#define CC1101_MARCSTATE         0x35        // Main Radio Control State Machine State
#define CC1101_WORTIME1          0x36        // High Byte of WOR Time
#define CC1101_WORTIME0          0x37        // Low Byte of WOR Time
#define CC1101_PKTSTATUS         0x38        // Current GDOx Status and Packet Status
#define CC1101_VCO_VC_DAC        0x39        // Current Setting from PLL Calibration Module
#define CC1101_TXBYTES           0x3A        // Underflow and Number of Bytes
#define CC1101_RXBYTES           0x3B        // Overflow and Number of Bytes
#define CC1101_RCCTRL1_STATUS    0x3C        // Last RC Oscillator Calibration Result
#define CC1101_RCCTRL0_STATUS    0x3D        // Last RC Oscillator Calibration Result 

/*****************************************************************************
 * MARC STATUS
 *****************************************************************************/

#define MARC_STATE_IDLE              0x01
#define MARC_STATE_RX                0x0D
#define MARC_STATE_RXFIFO_OV         0x11
#define MARC_STATE_TX                0x13
#define MARC_MARC_STATE_TX_END            0x14
#define MARC_STATE_RXTX_SWITCH       0x15
#define MARC_STATE_TXFIFO_UV         0x16

/*****************************************************************************
 * Macros
 *****************************************************************************/
// Enter Rx state
#define setRxState()              cmdStrobe(CC1101_SRX)
// Disable address check
#define disableAddressCheck()     writeReg(CC1101_PKTCTRL1, 0x04)
// Enable address check
#define enableAddressCheck()      writeReg(CC1101_PKTCTRL1, 0x06)
// Disable CCA
#define disableCCA()              writeReg(CC1101_MCSM1, 0)
// Enable CCA
#define enableCCA()               writeReg(CC1101_MCSM1, CC1101_DEFVAL_MCSM1)

#define CC1100_TX_RESULT_SUCCESS        0
#define CC1100_TX_RESULT_FAILED         1

/*****************************************************************************
 *  Configure and enable the SYNC signal interrupt.
 *  This interrupt is used to indicate receive.  The SYNC signal goes
 *  high when a receive OR a transmit begins.  It goes high once the
 *  sync word is received or transmitted and then goes low again once
 *  the packet completes.
*****************************************************************************/
#if(defined(RFID_READER))
  #define enableIRQ_GDO0()        (EIMSK |=  (1<<INT7))
  #define disableIRQ_GDO0()       (EIMSK &= ~(1<<INT7))
  #define clearIRQ_GDO0()         (INTFR  &= ~(1<<INT7));
#elif ((defined(RFID_TAG)))
  #define enableIRQ_GDO0()        (PCICR |=  (1<<PCIE0)); PCMSK0 |= (1 << PCINT6);   cbi(DDR_GD0,PIN_GD0);
  #define disableIRQ_GDO0()       (PCICR &= ~(1<<PCIE0)); PCMSK0 &= ~(1 << PCINT6);  cbi(DDR_GD0,PIN_GD0);
  #define clearIRQ_GDO0()         (PCIFR &= ~(1<<PCIE0));
#endif

/**
    name GDOx configuration constants
 */
#define CC1100_GDOx_RX_FIFO           0x00  /* assert above threshold, deassert when below         */
#define CC1100_GDOx_RX_FIFO_EOP       0x01  /* assert above threshold or EOP, deassert when empty  */
#define CC1100_GDOx_TX_FIFO           0x02  /* assert above threshold, deassert when below         */
#define CC1100_GDOx_TX_THR_FULL       0x03  /* asserts TX FIFO full. De-asserts when below thr     */
#define CC1100_GDOx_RX_OVER           0x04  /* asserts when RX overflow, deassert when flushed     */
#define CC1100_GDOx_TX_UNDER          0x05  /* asserts when RX underflow, deassert when flushed    */
#define CC1100_GDOx_SYNC_WORD         0x06  /* assert SYNC sent/recv, deasserts on EOP             */
                                            /* In RX, de-assert on overflow or bad address         */
                                            /* In TX, de-assert on underflow                       */
#define CC1100_GDOx_RX_OK             0x07  /* assert when RX PKT with CRC ok, de-assert on 1byte  */
                                            /* read from RX Fifo                                   */
#define CC1100_GDOx_PREAMB_OK         0x08  /* assert when preamble quality reached : PQI/PQT ok   */
#define CC1100_GDOx_CCA               0x09  /* Clear channel assessment. High when RSSI level is   */
                                            /* below threshold (dependent on the current CCA_MODE) */
#define CC1100_GDO_PA_PD                27  /* low when transmit is active, low during sleep */
#define CC1100_GDO_LNA_PD               28  /* low when receive is active, low during sleep */


/****************************************************************************************/
#define BIT_GDO0 2
/****************************************************************************************/
#define CS()                      cbi(PORTB, SPI_SS)
#define nCS()                     sbi(PORTB, SPI_SS)
#define MISO_WAIT()               LOOP_UNTIL_BIT_IS_LO(PINB,SPI_MISO); // Wait until SPI MISO line goes low

#define wait_GDO0_high()          LOOP_UNTIL_BIT_IS_HI(READ_GD0,PIN_GD0);// Wait until GDO0 line goes high
#define wait_GDO0_low()           LOOP_UNTIL_BIT_IS_LO(READ_GD0,PIN_GD0);// Wait until GDO0 line goes low

/*****************************************************************************/
typedef enum {
  eCC1100_TX_TYPE_FORCED = 1, eCC1100_TX_TYPE_CCA
} etxType;


/*****************************************************************************/
typedef struct  {
    u08 size;
    TAG::sRfTag tag;
    u08 rssi;
    u08 lqi;
    bool crc_ok;
} sRadioPacket;

/*****************************************************************************
 * Class: CC1101
 *****************************************************************************/
class CC1101 {
  private:
    SPI spi; //TODO hook in our own SPI class
    void readBurstReg(u08 * buffer, u08 regAddr, u08 len);
    void setupRegs(void);
    void setRegsFromEeprom(void);
    void setSyncWord(u08* sync);
    void setDevAddress(u08 addr);
    void setChannel(u08 chnl);
    void FlushRX(void);
    void FlushTX(void);
    u08 RxFifoEmpty(void);
    void goIdle(void);
  public:
    bool tagreceived;
    void rssiValidWait(void);
    u08 readReg(u08 regAddr, u08 regType);
    void writeReg(u08 regAddr, u08 value);
    u08 cmdStrobe(u08 cmd);
    void writeBurstReg(u08 regAddr, u08* buffer, u08 len);
#if(defined(FIFOS_ENABLED))
    Tfifo<sRadioPacket> rxFifo;
    Tfifo<sRadioPacket> txFifo;
#else
    sRadioPacket radioPckt;
#endif
    u08 channel;
    u08 syncWord[2];
    u08 devAddress;
    u08 rssi;
    u08 lqi;
    u08 crc_ok;
    u08 volatile gd0;
    CC1101(u08 addr, u08 chnl) {
      init();
      writeReg(CC1101_SYNC1, CC1101_DEFVAL_SYNC1);
      writeReg(CC1101_SYNC0, CC1101_DEFVAL_SYNC0);
      setDevAddress(addr);
      setChannel(chnl);
      gd0 = 1;
      tagreceived = 0;
#if(defined(FIFOS_ENABLED))
      rxFifo.setBufSize(1);
      txFifo.setBufSize(1);
#endif
    }

    void wait_PKTSTATUS(u08 mode);
    void wait_MARCSTATE(u08 mode);
    void transmit(u08 *data, u08 len, etxType txType);
    bool sendData1(u08 *data, u08 len);
    void init(void);
    void wakeUp(void);
    void reset(void);
    void powerdown();
    void receive(void);
    void ReceivePacket();
};
#endif

