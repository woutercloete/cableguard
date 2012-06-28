#ifndef PINS_H_
#define PINS_H_

#include <avr/io.h>
#include "common.h"

#define FRAM_CS_PORT            PORTB
#define FRAM_CS_DDR             DDRB
#define FRAM_CS_PORTIN          PINB
#define FRAM_CS_PIN             0

#define RS485_EN_PORT        PORTA
#define RS485_EN_DDR         DDRA
#define RS485_EN_PORTIN      PINA
#define RS485_EN_PIN         2

/*============== PORTA ================*/
#define LED0_PORT            PORTD
#define LED0_DDR             DDRD
#define LED0_PORTIN          PIND
#define LED0_PIN             4

#define LED1_PORT            PORTD
#define LED1_DDR             DDRD
#define LED1_PORTIN          PIND
#define LED1_PIN             5

#define LED2_PORT            PORTD
#define LED2_DDR             DDRD
#define LED2_PORTIN          PIND
#define LED2_PIN             6

#define LED3_PORT            PORTD
#define LED3_DDR             DDRD
#define LED3_PORTIN          PIND
#define LED3_PIN             7


#define LED0_ON()            BIT_SET_LO(LED0_PORT,LED0_PIN)
#define LED0_OFF()           BIT_SET_HI(LED0_PORT,LED0_PIN)
#define LED0_TOGGLE()        BIT_TOGGLE(LED0_PORT,LED0_PIN)

#define LED1_ON()            BIT_SET_LO(LED1_PORT,LED1_PIN)
#define LED1_OFF()           BIT_SET_HI(LED1_PORT,LED1_PIN)
#define LED1_TOGGLE()        BIT_TOGGLE(LED1_PORT,LED1_PIN)

#define LED2_ON()            BIT_SET_LO(LED2_PORT,LED2_PIN)
#define LED2_OFF()           BIT_SET_HI(LED2_PORT,LED2_PIN)
#define LED2_TOGGLE()        BIT_TOGGLE(LED2_PORT,LED2_PIN)

#define LED3_ON()            BIT_SET_LO(LED3_PORT,LED3_PIN)
#define LED3_OFF()           BIT_SET_HI(LED3_PORT,LED3_PIN)
#define LED3_TOGGLE()        BIT_TOGGLE(LED3_PORT,LED3_PIN)

#define RS485_INIT()        BIT_SET_HI(RS485_EN_DDR,RS485_EN_PIN)
#define RS485_TX()          BIT_SET_HI(RS485_EN_PORT,RS485_EN_PIN)
#define RS485_RX()          BIT_SET_LO(RS485_EN_PORT,RS485_EN_PIN)

void InitIOPins(void);

#endif

