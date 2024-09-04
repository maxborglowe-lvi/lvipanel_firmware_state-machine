#ifndef LIGHTS_H
#define LIGHTS_H

#include "tim.h"

#define LIGHT_RESOLUTION 32

static enum {
    LIMIT_NOT_REACHED,
    LIMIT_REACHED
};

typedef enum{
    LIGHT_STATE_ON,
    LIGHT_STATE_OFF,
} LightState;

typedef struct{
    LightState state;

    TIM_HandleTypeDef *tim;

    uint32_t *value;
    uint32_t value_limit;
    
    uint32_t value_increment;

} Light;

void Lights_Init(Light *light, TIM_HandleTypeDef *tim, uint8_t channel);

uint8_t Lights_LinearIncrease(Light *light);
uint8_t Lights_LinearDecrease(Light *light);

#endif // LIGHTS_H