/******************************************************************************
Scheduler
******************************************************************************/
#include "scheduler.h"
#include "avrlibtypes.h"
#include "rprintf.h"
#include "timer1280.h"
#include "drv_ioctrl.h"

#undef DEBUG_SCHEDULER


/* 50ms tick rate */
#define SCHED_DIVISOR 	11



#define MAX_SIGNALS   8

static u08  schedTicks;
static volatile u08* signal[MAX_SIGNALS];
static volatile u08  period[MAX_SIGNALS];
static u08  ticks[MAX_SIGNALS];

u08 schedInit()
{
  u08 cnt;
  
  timerAttach(TIMER1OVERFLOW_INT, schedService);
  schedTicks = 0;
  for (cnt = 0 ; cnt < MAX_SIGNALS ; cnt++)
  {
    signal[cnt] = 0;
  }
  
  return 0;
}

u08 schedAttach(volatile u08* _signal, u08 _period)
{
  u08 ret;
  u08 cnt;
  u08 found;
  
  ret = 1;
  cnt = 0;
  found = false;

  while ((cnt < MAX_SIGNALS) && (found == false))
  {
    if (signal[cnt] == 0)
    {
      signal[cnt] = _signal;
      *signal[cnt] = false;
      period[cnt] = _period;
      ticks[cnt] = 0;
      ret = 0;
      found = true;
    }
    cnt++;
  }
  #ifdef DEBUG_SCHEDULER
  rprintfProgStrM("\r\n schedAttach %u", ret);
  #endif
 
  return ret;
}

void schedService()
{
  u08 cnt;
  schedTicks = schedTicks + 1;
  if (schedTicks >=  SCHED_DIVISOR)
  {
	LED_0_TOGGLE();
	schedTicks = 0;
    for (cnt = 0 ; cnt < MAX_SIGNALS ; cnt++)
    {
			if (signal[cnt] != 0)
      {
        ticks[cnt]++;
        if (ticks[cnt] >= period[cnt])
        {
          #ifdef DEBUG_SCHEDULER
					rprintf("\n\rticks[%d]",ticks[cnt]);      
					#endif
					*signal[cnt] = true;
          ticks[cnt] = 0;
        }
      }
    }
  }
}
   
