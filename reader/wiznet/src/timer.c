/*
*
@file		timer.c
*
*/

#include <stdio.h> 
#include <string.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

#include "types.h"
#include "socket.h"


#include "timer.h"

#define TCNT0_VALUE	0xD7		/**< 5ms at 8MHz */
#define TCNT2_VALUE	0x87		/**< 15ms at 8MHz */


#if (MAX_TIMER0_CNT > 0)
static int sys_tick_5ms = 0;
#endif

#if (MAX_TIMER2_CNT > 0)
static int sys_tick_1s = 0;		/**< Tick count */
#endif


/**
 @brief	Timer handler table 
*/
struct _TIMER_IRQS
{
	TIMER_IRQ timer_irq[MAX_TIMER_CNT];
}TIMER_IRQS;


#if (MAX_TIMER0_CNT > 0)
#if (__COMPILER_VERSION__ == __WINAVR_20050214__)
static void timer0_irq( void );
void SIG_OVERFLOW0( void ) __attribute__ ((signal));

void SIG_OVERFLOW0( void )
{
	timer0_irq();
}

/**
@brief	This function counts timer tick value.
 
Internal Function \n
Interrupt Service Routine for AVR GCC ver. 3.4.6
*/
static void timer0_irq( void )
{ 
	u_char i;
	sys_tick_5ms++;
	for(i=0; i < MAX_TIMER0_CNT; i++)
	{
		if(TIMER_IRQS.timer_irq[i].user_timer_handler)
			(*TIMER_IRQS.timer_irq[i].user_timer_handler)();
	}

	TCNT0 = TCNT0_VALUE;			/* reset TCNT0(5 ms) */
}
#else
/**
@brief	This function counts timer tick value.
 
Internal Function \n
Interrupt Service Routine for AVR GCC ver. 3.4.6
*/
ISR(TIMER0_OVF_vect)
{ 
	u_char i;
	sys_tick_5ms++;
	for(i=0; i < MAX_TIMER0_CNT; i++)
	{
		if(TIMER_IRQS.timer_irq[i].user_timer_handler)
			(*TIMER_IRQS.timer_irq[i].user_timer_handler)();
	}

	TCNT0 = TCNT0_VALUE;			/* reset TCNT0(5 ms) */
}
#endif
#endif


#if (MAX_TIMER2_CNT > 0)
#if (__COMPILER_VERSION__ == __WINAVR_20050214__)
static void timer2_irq( void );
void SIG_OVERFLOW2( void ) __attribute__ ((signal));

void SIG_OVERFLOW2( void )
{
	timer2_irq();
}

/**
@brief	This function counts timer tick value.
*/
static void timer2_irq( void )
{
	u_char i;
	
	if (++sys_tick_1s > 64) /* 1 sec. */
	{	
		sys_tick_1s = 0;
		for(i=MAX_TIMER0_CNT; i < MAX_TIMER_CNT; i++)
		{
			if(TIMER_IRQS.timer_irq[i].user_timer_handler)
				(*TIMER_IRQS.timer_irq[i].user_timer_handler)();
		}
	}
	TCNT2 = TCNT2_VALUE;		/* reset TCNT0(15 ms) */
}
#else
/**
@brief	This function counts timer tick value.
  
Internal Function \n
Interrupt Service Routine for AVR GCC ver. 3.4.6
*/
ISR(TIMER2_OVF_vect)
{
	u_char i;

	if (++sys_tick_1s > 64) /* 1 sec. */
	{	
		sys_tick_1s = 0;
		for(i=MAX_TIMER0_CNT; i < MAX_TIMER_CNT; i++)
		{
			if(TIMER_IRQS.timer_irq[i].user_timer_handler)
				(*TIMER_IRQS.timer_irq[i].user_timer_handler)();
		}
	}
	TCNT2 = TCNT2_VALUE;		/* reset TCNT0(15 ms) */
}
#endif	/* #ifdef __STDIO_FDEVOPEN_COMPAT_12 */
#endif	/* #if (MAX_TIMER2_CNT > 0) */


/**
@brief	This function initialize AVR timer.
*/
void init_timer(void)
{
#if ( MAX_TIMER_CNT > 0 )
	u_char i;
	
	cli();
	for(i=0;i<MAX_TIMER_CNT; i++)
		TIMER_IRQS.timer_irq[i].user_timer_handler = 0;
#if (MAX_TIMER2_CNT > 0)	
	TIMSK &= ~(1 << TOIE2);	
	TCNT2 = TCNT2_VALUE;		/* reset TCNT2(15 ms) */
	TCCR2 = 5;		/* count with cpu clock/1024 */
	sys_tick_1s = 0;
#endif	

#if (MAX_TIMER0_CNT > 0)
	TIMSK &= ~(1 << TOIE0);		/* disable TCNT0 overflow */
	TCNT0 = TCNT0_VALUE;			/* reset TCNT0(5 ms) */
	TCCR0 = 7;			/* count with cpu clock/1024 */		
	sys_tick_5ms = 0;
#endif	
	sei();
#endif	/* #if ( MAX_TIMER_CNT > 0 ) */
}


/**
@brief	Register the timer handler
*/
void set_timer(
	u_int timer, 			/**< timer Handler Number */
	void (*handler)(void)	/**< user specific function to be called by timer interrupt */
	) 
{
#if ( MAX_TIMER_CNT > 0 )
	cli();
#if (MAX_TIMER0_CNT > 0)	
	if(timer < MAX_TIMER0_CNT)
	{
		TIMSK |= (1 << TOIE0);
	}	
	else
#endif		
	{
#if (MAX_TIMER2_CNT > 0)		
		TIMSK |= (1 << TOIE2);
#endif
	}
	
	if(handler) TIMER_IRQS.timer_irq[timer].user_timer_handler = handler;
	sei();
#endif /* #if ( MAX_TIMER_CNT > 0 ) */
}


/**
@brief	Unregister Timer Handler
*/
void kill_timer(
	u_int timer	/**< user specific function to be called by timer interrupt */
	) 
{
#if ( MAX_TIMER_CNT > 0 )
	u_char i;
	u_char bkill;
	cli();
	TIMER_IRQS.timer_irq[timer].user_timer_handler = 0;

#if (MAX_TIMER0_CNT > 0)
	if(timer < MAX_TIMER0_CNT)
	{
		bkill = 1;
		for(i = 0; i < MAX_TIMER0_CNT; i++)
		{
			if(TIMER_IRQS.timer_irq[i].user_timer_handler)
			{
				bkill = 0;
				break;
			}
		}
		if(bkill) 
		{
			sys_tick_5ms = 0;
			TIMSK &= ~(1<<TOIE0);		
		}
	}
	else
#endif		
	{
#if (MAX_TIMER2_CNT > 0)	
		bkill = 1;
		for(i = MAX_TIMER0_CNT; i < MAX_TIMER_CNT; i++)
		{
			if(TIMER_IRQS.timer_irq[i].user_timer_handler)
			{
				bkill = 0;
				break;
			}
		}
		if(bkill)
		{
			TIMSK &= ~(1<<TOIE2);
			sys_tick_1s = 0;
		}
#endif		
	}
	sei();
#endif /* #if ( MAX_TIMER_CNT > 0 ) */
}
