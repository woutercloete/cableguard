//======================================================================
#ifndef RFIDDEVICE_H
#define RFIDDEVICE_H
//======================================================================
#include "global.h" 
//======================================================================
#define PING_RECEIVER                       0x10 
#define GET_TAG_PACKET                      0x11
#define GET RECEIVER_STATUS                 0x12
#define GET_RECEIVER_ID                     0x13
#define GET_RECEIVER_RSSI                   0x14
#define GET_RECEIVER_CUSTOMER_CODE          0x15
#define GET_RECEIVER_MODE                   0x16
#define GET_RECEIVER_IO                     0x17
#define GET_RECEIVER_RANGE_STATUS           0x18
#define GET_RECEIVER_ALARMS_MODE            0x19
#define GET_RECEIVER_FIRMWARE_VERSION       0x1A
#define GET_RECEIVER_VOLTAGE                0x21
#define GET_RECEIVER_RSSI_OFFSET            0x22
#define GET_RECEIVER_LOW_VOLTAGE_LEVEL      0x23
#define GET_RECEIVER_BUFFER_SIZE            0x24
#define GET_RECEIVER_BUFFER_COUNT           0x25
#define GET_WIEGAND_BUFFER_TIME             0x26
#define GET_BUFFER_TIMEOUT                  0x27
#define GET_SERIAL_NUMBER                   0x28
#define GET_TCP_TIME                        0x29
#define GET_RAM_INDEX                       0x2A
#define GET_RAM DATA                        0x2B
#define GET_DURESS_OFFSET                   0x2C
#define GET_TAG_DATA_ENABLE                 0x2D
#define GET_RECEIVER_SECONDS                0x2E
#define GET_TIME_DATE                       0x30
//======================================================================
#define SET_RECEIVER_ID                     0x31
#define SET_RECEIVER_RSSI                   0x32
#define SET_RECEIVER_CUSTOMER_CODE          0x33
#define SET_RECEIVER_MODE                   0x34
#define SET_RECEIVER_LOW_VOLTAGE_LEVEL      0x35
#define SET_RECEIVER_IO                     0x36
#define SET_RECEIVER_IGNORE_LR_TX_ON_OFF    0x37
#define SET_RECEIVER_ALARMS_MODE_ON_OFF     0x38
#define SET_RECEIVER_BAUD_RATE              0x39
#define SET_RECEIVER_RF_ON_OFF              0x40
#define SET_RECEIVER_RSSI_OFFSET            0x41
#define SET_RECEIVER_BUFFER SIZE            0x43
#define SET_WIEGAND_BUFFER_TIME             0x44
#define SET_BUFFER_TIMEOUT                  0x45
#define SET_SERIAL_NUMBER                   0x45
#define SET_TCP_TIME                        0x47
#define SET_TIME_DATE                       0x48
#define SET_RAM_INDEX                       0x49
#define SET_RAM_ERASE                       0x4A
#define SET_RECEIVER_OFF                    0x4B
//======================================================================
#define TAG_LOST                            1
#define TAG_PRESENT                         2
#define TAG_DURESS                          3
//======================================================================
typedef enum
{
  TR_SEEN = 1,
  TR_GONE = 2,
  TR_DURESS = 3
} TagStateRFID;
//======================================================================
void InitRFID (void);
void ServiceRFID (void);
void ServiceTagTable(void);
TagStateRFID GetTagStateRFID(u32 RFID);
//======================================================================
#endif

