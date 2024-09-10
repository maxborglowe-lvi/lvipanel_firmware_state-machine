#ifndef LIGHTS_H
#define LIGHTS_H

#include "tim.h"

#define LIGHT_RESOLUTION 32

static enum {
    LIMIT_NOT_REACHED,
    LIMIT_REACHED
};

typedef enum {
    LIGHT_STATE_ON,
    LIGHT_STATE_OFF,
} LightState;

typedef struct {
    LightState state;

    TIM_HandleTypeDef *tim;

    uint32_t *pwm_value;        // Pointer to the PWM value register
    uint32_t max_pwm_value;     // Maximum PWM value (value_limit)

    uint32_t pwm_step;          // Increment for linear adjustments

    float current_brightness;   // Current brightness level (0.0 to 1.0)
    int fade_direction;         // 1 for fading in, -1 for fading out
} Light;

void Lights_Init(Light *light, TIM_HandleTypeDef *tim, uint8_t channel, uint32_t limit);

uint8_t Lights_LinearIncrease(Light *light);
uint8_t Lights_LinearDecrease(Light *light);

void Lights_SmoothFade(Light *light, float fade_speed);
uint8_t Lights_FadeIn(Light *light, float fade_speed);
uint8_t Lights_FadeOut(Light *light, float fade_speed);
void Lights_SetValue(Light *light, float value_amt);

#endif // LIGHTS_H
