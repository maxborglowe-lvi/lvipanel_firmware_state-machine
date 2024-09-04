#include "lights.h"



void Lights_Init(Light *light, TIM_HandleTypeDef *tim, uint8_t channel){
    light->state = LIGHT_STATE_ON;

    light->tim = tim;

    light->value_limit = tim->Instance->ARR;

    light->value_increment = light->value_limit/LIGHT_RESOLUTION;

    switch (channel)
    {
    case 1:
        light->value = (uint32_t *)&tim->Instance->CCR1;
        HAL_TIM_PWM_Start(tim, TIM_CHANNEL_1);
        break;

    case 2:
        light->value = (uint32_t *)&tim->Instance->CCR2;
        HAL_TIM_PWM_Start(tim, TIM_CHANNEL_2);
        break;

    case 3:
        light->value = (uint32_t *)&tim->Instance->CCR3;
        HAL_TIM_PWM_Start(tim, TIM_CHANNEL_3);
        break;

    case 4:
        light->value = (uint32_t *)&tim->Instance->CCR4;
        HAL_TIM_PWM_Start(tim, TIM_CHANNEL_4);
        break;
    
    default:
        break;
    }
}

uint8_t Lights_LinearIncrease(Light *light){
    *(light->value) += light->value_increment;
    if (*(light->value) > light->value_limit) {
        *(light->value) = light->value_limit;
        LIMIT_REACHED;
    }
    return LIMIT_NOT_REACHED;
}

uint8_t Lights_LinearDecrease(Light *light){

    if (*(light->value) >= light->value_increment) {
        *(light->value) -= light->value_increment;
        return LIMIT_NOT_REACHED;
    } else {
        *(light->value) = 0;
        return LIMIT_REACHED;
    }
}