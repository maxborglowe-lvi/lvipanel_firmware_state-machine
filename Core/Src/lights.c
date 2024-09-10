#include "lights.h"

void Lights_Init(Light *light, TIM_HandleTypeDef *tim, uint8_t channel, uint32_t limit) {
    light->state = LIGHT_STATE_ON;

    light->tim = tim;

    if (limit > tim->Instance->ARR)
        light->max_pwm_value = tim->Instance->ARR;
    else
        light->max_pwm_value = limit;

    light->pwm_step = light->max_pwm_value / LIGHT_RESOLUTION;

    switch (channel) {
    case 1:
        light->pwm_value = (uint32_t *)&tim->Instance->CCR1;
        HAL_TIM_PWM_Start(tim, TIM_CHANNEL_1);
        break;

    case 2:
        light->pwm_value = (uint32_t *)&tim->Instance->CCR2;
        HAL_TIM_PWM_Start(tim, TIM_CHANNEL_2);
        break;

    case 3:
        light->pwm_value = (uint32_t *)&tim->Instance->CCR3;
        HAL_TIM_PWM_Start(tim, TIM_CHANNEL_3);
        break;

    case 4:
        light->pwm_value = (uint32_t *)&tim->Instance->CCR4;
        HAL_TIM_PWM_Start(tim, TIM_CHANNEL_4);
        break;
    
    default:
        break;
    }
}

uint8_t Lights_LinearIncrease(Light *light) {
    *(light->pwm_value) += light->pwm_step;
    if (*(light->pwm_value) > light->max_pwm_value) {
        *(light->pwm_value) = light->max_pwm_value;
        return LIMIT_REACHED;
    }
    return LIMIT_NOT_REACHED;
}

uint8_t Lights_LinearDecrease(Light *light) {
    if (*(light->pwm_value) >= light->pwm_step) {
        *(light->pwm_value) -= light->pwm_step;
        return LIMIT_NOT_REACHED;
    } else {
        *(light->pwm_value) = 0;
        return LIMIT_REACHED;
    }
}

void Lights_SetValue(Light *light, float value_amt) {
    if (value_amt < 0) return;
    if (value_amt > 1) return;

    *(light->pwm_value) = light->max_pwm_value * value_amt;
}

void Lights_SmoothFade(Light *light, float fade_speed) {
    // Initialize if not done yet
    if (light->current_brightness == 0.0f && light->fade_direction == 0) {
        light->current_brightness = (float)(*(light->pwm_value)) / (float)(light->max_pwm_value);
        light->fade_direction = 1;  // Start by increasing
    }

    // Calculate the change in brightness per step based on fade_speed
    float brightness_increment = fade_speed / 1000.0f;  // Adjust fade_speed for 1ms intervals

    // Update the brightness value based on the current direction
    light->current_brightness += light->fade_direction * brightness_increment;

    // Check if the brightness has reached the limits (0 or 1) and reverse the direction
    if (light->current_brightness >= 1.0f) {
        light->current_brightness = 1.0f;  // Cap at max brightness
        light->fade_direction = -1;        // Start decreasing
    } else if (light->current_brightness <= 0.0f) {
        light->current_brightness = 0.0f;  // Cap at min brightness
        light->fade_direction = 1;         // Start increasing
    }

    // Set the light's value using the percentage function
    Lights_SetValue(light, light->current_brightness);
}

uint8_t Lights_FadeIn(Light *light, float fade_speed) {
    // Initialize if not done yet
    if (light->current_brightness == 0.0f) {
        light->current_brightness = (float)(*(light->pwm_value)) / (float)(light->max_pwm_value);
    }

    // Calculate the increment based on fade_speed
    float brightness_increment = fade_speed / 1000.0f;  // Adjust for 1ms intervals

    // Increase the current brightness value
    light->current_brightness += brightness_increment;

    // Cap the value at 100% (1.0)
    if (light->current_brightness >= 1.0f) {
        light->current_brightness = 1.0f;  // Set to max brightness
        Lights_SetValue(light, light->current_brightness);
        return LIMIT_REACHED;  // Indicate fade-in is complete
    }

    // Set the light's brightness
    Lights_SetValue(light, light->current_brightness);
    return LIMIT_NOT_REACHED;  // Indicate fade-in is still in progress
}

uint8_t Lights_FadeOut(Light *light, float fade_speed) {
    // Initialize if not done yet
    if (light->current_brightness == 0.0f) {
        light->current_brightness = (float)(*(light->pwm_value)) / (float)(light->max_pwm_value);
    }

    // Calculate the decrement based on fade_speed
    float brightness_decrement = fade_speed / 1000.0f;  // Adjust for 1ms intervals

    // Decrease the current brightness value
    light->current_brightness -= brightness_decrement;

    // Cap the value at 0% (0.0)
    if (light->current_brightness <= 0.0f) {
        light->current_brightness = 0.0f;  // Set to min brightness
        Lights_SetValue(light, light->current_brightness);
        return LIMIT_REACHED;  // Indicate fade-out is complete
    }

    // Set the light's brightness
    Lights_SetValue(light, light->current_brightness);
    return LIMIT_NOT_REACHED;  // Indicate fade-out is still in progress
}
