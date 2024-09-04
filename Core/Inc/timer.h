/*
 * timer.h
 *
 *  Created on: Oct 10, 2022
 *      Author: mb2
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "stdint.h"
#include "stm32f0xx_hal.h"

#define TIM_SLEEP 0x00
#define TIM_ALARM 0x01

/**Interrupt-triggered timer used for timed functions in the panel */
typedef struct {
	uint8_t enable;		/* Determines whether the timer is enabled (1) or disabled (0) */
	uint32_t elapsed; 	/* Internal counter triggered by interrupt */
	uint32_t goal;		/* The value at which the timer resets and triggers */

	GPIO_TypeDef *PORT;	/* Button port */
	uint16_t PIN; 		/* Button input pin */
} Timer;

void timerInit(Timer *tim, uint32_t goal);
void timerEnable(Timer *tim);
void timerDisable(Timer *tim);
uint8_t timerIsEnabled(Timer *tim);
uint8_t timerCountUp(Timer *tim);
void timerReset(Timer *tim);


#endif /* INC_TIMER_H_ */
