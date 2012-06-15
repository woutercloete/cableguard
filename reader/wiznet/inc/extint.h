#ifndef EXTINT_H
#define EXTINT_H

#define EXTINT0					0x00	///< External Interrupt 0
#define EXTINT1					0x01	///< External Interrupt 1
#define EXTINT2					0x02	///< External Interrupt 2
#define EXTINT3					0x03	///< External Interrupt 3
#define EXTINT4					0x04	///< External Interrupt 4
#define EXTINT5					0x05	///< External Interrupt 5
#define EXTINT6					0x06	///< External Interrupt 6
#define EXTINT7					0x07	///< External Interrupt 7

#define EXTINT_PC0				0x00	///< External Pin Change Interrupt Group 0
#define EXTINT_PC1				0x01	///< External Pin Change Interrupt Group 1
#define EXTINT_PC2				0x02	///< External Pin Change Interrupt Group 2
#define EXTINT_PC3				0x03	///< External Pin Change Interrupt Group 3

#define EXTINT_LEVEL_LOW		0x00	///< Trigger on low level
#define EXTINT_EDGE_ANY			0x01	///< Trigger on any edge
#define EXTINT_EDGE_FALLING		0x02	///< Trigger on falling edge
#define EXTINT_EDGE_RISING		0x03	///< Trigger on rising edge

// type of interrupt handler to use
// *do not change unless you know what you're doing
// Value may be SIGNAL or INTERRUPT
#ifndef EXTINT_INTERRUPT_HANDLER
#define EXTINT_INTERRUPT_HANDLER	SIGNAL
#endif

// processor-adaptive defines
// mainstream AVR processors generally have 1,2,3, or 8 external interrupts
// (if someone has a better idea of how to manage this, let me know)
#ifdef SIG_INTERRUPT7
#define EXTINT_NUM_INTERRUPTS	8
#else
#ifdef SIG_INTERRUPT2
#define EXTINT_NUM_INTERRUPTS	3
#else
#ifdef SIG_INTERRUPT1
#define EXTINT_NUM_INTERRUPTS	2
#else
#define EXTINT_NUM_INTERRUPTS	1
#endif
#endif
#endif

#ifdef SIG_PIN_CHANGE3
  #define PINCHANGE_NUM_INTERRUPTS 4
#else
  #ifdef SIG_PIN_CHANGE2
    #define PINCHANGE_NUM_INTERRUPTS 3
  #else
    #ifdef SIG_PIN_CHANGE2
      #define PINCHANGE_NUM_INTERRUPTS 1
    #else
      #define PINCHANGE_NUM_INTERRUPTS 1
    #endif
  #endif
#endif

typedef enum {
	eTypeExt, eTypePin
} eIntType;

typedef void (*voidFuncPtr)(void*);

void extintInit(void);
void extintConfigure(u08 interruptNum, u08 configuration);
void pinintConfigure(u08 pinGroupNum, u08 pinNum);
void extintAttach(u08 interruptNum, void(*userHandler)(void*), void* object, eIntType type);
void extintDetach(u08 interruptNum, eIntType type);

#endif
