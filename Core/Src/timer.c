/*
 * timer.c
 *
 *  Created on: Oct 10, 2022
 *      Author: mb2
 */

#include "timer.h"

/**
 * @brief Timer initialization process.
 * The timer counter (elapsed) is reset at init, and starts disabled.
 * @param goal: The value at which the timer will trigger an alarm (TIM_ALARM) */
void timerInit(Timer *tim, uint32_t goal){
	tim->elapsed = 0;
	tim->goal = goal;
	tim->enable = 1;
}

/**
 * @brief Enables timer.
 */
void timerEnable(Timer *tim){
	tim->enable = 1;
}

/**
 * @brief Disables timer.
 */
void timerDisable(Timer *tim){
	tim->enable = 0;
	tim->elapsed = 0;
}


/**
 * @retval 0 = disabled, 1 = enabled
 * */
uint8_t timerIsEnabled(Timer *tim){
	return tim->enable;
}

/**
 * @brief Increments and polls the timer.
 * @return 	TIM_SLEEP (0) = The timer has not reached its goal.
 * 			TIM_ALARM (1) = The timer has reached its goal.
 */
uint8_t timerCountUp(Timer *tim){
	if(tim->enable){
		if(tim->elapsed == tim->goal){
			tim->elapsed = 0;
			return TIM_ALARM;
		}
		tim->elapsed++;
	}
	return TIM_SLEEP;
}

void timerReset(Timer *tim){
	tim->elapsed = 0;
}

