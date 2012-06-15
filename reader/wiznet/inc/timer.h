/*
*
@file		timer.h
@brief	DHCP Timer related functions. (AVR-GCC Compiler)
*
*/
#ifndef __TIMER_H__
#define __TIMER_H__


#define MAX_TIMER0_CNT	0	// MAX Timer0 Handler Count
#define MAX_TIMER2_CNT	1	// MAX Timer2 Handler Count
#define MAX_TIMER_CNT	(MAX_TIMER0_CNT+MAX_TIMER2_CNT)	// Max Timer Handler

#define DHCP_CHECK_TIMER2	0

/* Timer handler */
typedef struct _TIMER_IRQ
{
	void (*user_timer_handler)(void);
}TIMER_IRQ;




extern void init_timer(void);

extern void set_timer(u_int timer, void (*handler)(void));

extern void kill_timer(u_int timer);

#if 0
extern void timer0_irq(void);
extern void timer2_irq(void);
#endif

#endif
