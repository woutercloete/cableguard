/* Sync word qualifier mode = 30/32 sync word bits detected */
/* CRC autoflush = false */
/* Channel spacing = 199.951172 */
/* Data format = Normal mode */
/* Data rate = 38.3835 */
/* RX filter BW = 101.562500 */
/* PA ramping = false */
/* Preamble count = 4 */
/* Address config = No address check */
/* Whitening = true */
/* Carrier frequency = 433.999969 */
/* Device address = 0 */
/* TX power = 0 */
/* Manchester enable = false */
/* CRC enable = true */
/* Deviation = 20.629883 */
/* Modulation format = GFSK */
/* Base frequency = 433.999969 */
/* Modulated = true */
/* Channel number = 0 */
/***************************************************************
 *  SmartRF Studio(tm) Export
 *
 *  Radio register settings specifed with C-code
 *  compatible #define statements.
 *
 *  RF device: CC1101
 *
 ***************************************************************/

#ifndef SMARTRF_CC1101_H
#define SMARTRF_CC1101_H

#define SMARTRF_RADIO_CC1101
#define CC1101_DEFVAL_IOCFG2           0x29
#define CC1101_DEFVAL_IOCFG1           0x2E
#define CC1101_DEFVAL_IOCFG0           0x06
#define CC1101_DEFVAL_FIFOTHR          0x47
#define CC1101_DEFVAL_SYNC1            0xD3
#define CC1101_DEFVAL_SYNC0            0x91
#define CC1101_DEFVAL_PKTLEN           0x20
#define CC1101_DEFVAL_PKTCTRL1         0x04
#define CC1101_DEFVAL_PKTCTRL0         0x45
#define CC1101_DEFVAL_ADDR             0x00
#define CC1101_DEFVAL_CHANNR           0x00
#define CC1101_DEFVAL_FSCTRL1          0x08
#define CC1101_DEFVAL_FSCTRL0          0x00
#define CC1101_DEFVAL_FREQ2            0x10
#define CC1101_DEFVAL_FREQ1            0xB1
#define CC1101_DEFVAL_FREQ0            0x3B
#define CC1101_DEFVAL_MDMCFG4          0xCA
#define CC1101_DEFVAL_MDMCFG3          0x83
#define CC1101_DEFVAL_MDMCFG2          0x93
#define CC1101_DEFVAL_MDMCFG1          0x22
#define CC1101_DEFVAL_MDMCFG0          0xF8
#define CC1101_DEFVAL_DEVIATN          0x35
#define CC1101_DEFVAL_MCSM2            0x07
#define CC1101_DEFVAL_MCSM1            0x00
#define CC1101_DEFVAL_MCSM0            0x18
#define CC1101_DEFVAL_FOCCFG           0x16
#define CC1101_DEFVAL_BSCFG            0x6C
#define CC1101_DEFVAL_AGCCTRL2         0x43
#define CC1101_DEFVAL_AGCCTRL1         0x40
#define CC1101_DEFVAL_AGCCTRL0         0x91
#define CC1101_DEFVAL_WOREVT1          0x87
#define CC1101_DEFVAL_WOREVT0          0x6B
#define CC1101_DEFVAL_WORCTRL          0xFB
#define CC1101_DEFVAL_FREND1           0x56
#define CC1101_DEFVAL_FREND0           0x10
#define CC1101_DEFVAL_FSCAL3           0xE9
#define CC1101_DEFVAL_FSCAL2           0x2A
#define CC1101_DEFVAL_FSCAL1           0x00
#define CC1101_DEFVAL_FSCAL0           0x1F
#define CC1101_DEFVAL_RCCTRL1          0x41
#define CC1101_DEFVAL_RCCTRL0          0x00
#define CC1101_DEFVAL_FSTEST           0x59
#define CC1101_DEFVAL_PTEST            0x7F
#define CC1101_DEFVAL_AGCTEST          0x3F
#define CC1101_DEFVAL_TEST2            0x81
#define CC1101_DEFVAL_TEST1            0x35
#define CC1101_DEFVAL_TEST0            0x09
#define CC1101_DEFVAL_PARTNUM          0x00
#define CC1101_DEFVAL_VERSION          0x04
#define CC1101_DEFVAL_FREQEST          0x00
#define CC1101_DEFVAL_LQI              0x00
#define CC1101_DEFVAL_RSSI             0x80
#define CC1101_DEFVAL_MARCSTATE        0x01
#define CC1101_DEFVAL_WORTIME1         0x00
#define CC1101_DEFVAL_WORTIME0         0x00
#define CC1101_DEFVAL_PKTSTATUS        0x00
#define CC1101_DEFVAL_VCO_VC_DAC       0x94
#define CC1101_DEFVAL_TXBYTES          0x00
#define CC1101_DEFVAL_RXBYTES          0x00
#define CC1101_DEFVAL_RCCTRL1_STATUS   0x00
#define CC1101_DEFVAL_RCCTRL0_STATUS   0x00

#endif
