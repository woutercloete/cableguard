/****************************************************************************************/
#include "scheduler.h"
/****************************************************************************************/
#undef DEBUG_SCHEDULER
/****************************************************************************************/
static Ctimer timer(0, DIVISOR, CONTINUOUS);
/****************************************************************************************/
Csignal* signals[SCHEDULER::MAX_SIGNALS];
Cscheduler::Cscheduler scheduler;
/****************************************************************************************/
Cscheduler::Cscheduler(void) {
  u08 cnt;
  timer.attach(service);
  for (cnt = 0; cnt < MAX_SIGNALS; cnt++) {
    signals[cnt] = 0;
  }
}
/****************************************************************************************/
void Cscheduler::start(void) {
  timer.enableInterrupt(OVERFLOW);
  timer.start(PERIOD_CNT);
}
/****************************************************************************************/
bool Cscheduler::attach(Csignal* _signal) {
  u08 cnt = 0;
  while (cnt < MAX_SIGNALS) {
    if (signals[cnt] == 0) {
      signals[cnt] = _signal;
      return true;
    }
    cnt++;
  }
  return false;
}
/****************************************************************************************/
void Cscheduler::service(void) {
  u08 cnt;
  for (cnt = 0; cnt < MAX_SIGNALS; cnt++) {
    if (signals[cnt] != 0) {
      signals[cnt]->tick();
    }
  }
}
/****************************************************************************************/

