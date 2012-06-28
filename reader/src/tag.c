/*****************************************************************************/
#include <string.h>
#include <avr/eeprom.h>
/*****************************************************************************/
#include "rprintf.h"
#include "tag.h"
#include "drv_rfid.h"
#include "debug.h"
/*****************************************************************************/
#define AT_WAIT_TIME        10
#define AT_WAIT_AT_STARTUP  30
/*****************************************************************************/
u08 tagInit(sTag* this)
{
  this->RFID = 0;
  this->State = TAG_UNUSED;
  this->Logged = false;
  this->Displayed = false;
  this->SignalDuress = FALSE;
  return 0;
}
/*****************************************************************************/
u08 tagSetID(sTag* this, u32 RFID)
{
  this->RFID = RFID;
  return 0;
}
/*****************************************************************************/
u08 tagSetName(sTag* this, c08* Name)
{
  if (strlen(Name) < MAX_NAME_LEN)
  {
    strncpy(this->Name, Name, MAX_NAME_LEN);
  }
  else
  {
    strcpy(this->Name, "Error");
    return 1;
  }
  return 0;
}
/*****************************************************************************/
u08 tagGetName(sTag* this, c08* Name)
{
  strcpy(Name, this->Name);
  return 0;
}
/*****************************************************************************/
u08 tagService(sTag* this)
{
  TagStateRFID TagState;
  
  if (this->State != TAG_UNUSED)
  {
    TagState = GetTagStateRFID(this->RFID);
    
    switch(this->State)
    {
      case TAG_UNUSED:
        break;
      case TAG_IN_RANGE:
        if (TagState == TR_DURESS)
        {
          this->SignalDuress = TRUE;
          this->State = TAG_DUR;
          this->Logged = false;
          this->Displayed = false;
        }
        if (TagState == TR_GONE)
        {
          this->State = TAG_OUT_RANGE;
          this->Logged = false;
          this->Displayed = false;
        }
        break;
      case TAG_OUT_RANGE:
        if (TagState == TR_DURESS)
        {
          this->SignalDuress = TRUE;
          this->State = TAG_DUR;
          this->Logged = false;
          this->Displayed = false;
        }
        if (TagState == TR_SEEN)
        {
          this->State = TAG_IN_RANGE;
          this->Logged = false;
          this->Displayed = false;
        }
        break;
      case TAG_DUR:
        if (TagState == TR_GONE)
        {
          this->State = TAG_OUT_RANGE;
          this->Logged = false;
          this->Displayed = false;
        }
        if (TagState == TR_SEEN)
        {
          this->State = TAG_IN_RANGE;
          this->Logged = false;
          this->Displayed = false;
        }
        break;
    }
    return true;
  }
  return false;
}
