/**
 * @file panel.c
 * @brief Handles panel events, system state transitions, and light control.
 *
 * This module is responsible for initializing the panel, handling peripheral
 * events, system events, and controlling the state of various system LEDs based
 * on the system's current state and flags.
 */

#include "panel.h"
#include "visca.h"

// Global variables for lights, timer, events, and system state
Light light_red;         ///< Red light control object
Light light_green;       ///< Green light control object
Timer timer_main;        ///< Main timer object
PanelEvent panelEvent;   ///< Current panel event
SystemEvent systemEvent; ///< Current system event
SystemState systemState; ///< Current system state
SystemState systemStatePrev;
SystemFlag systemFlag;     ///< Current system flag
SystemFlag systemFlagPrev; ///< Previous system flag for change detection

uint8_t PanelOnOffTrigger = 0;

Timer timerPanelOnOff;

GPIO_InitTypeDef GPIO_InitStruct = {0};

uint8_t yellowSyncComplete = 0;

uint8_t zoom_current = 0;
uint8_t zoom_saved = 0;
uint8_t overview_active = 0;

/**
 * @brief Initializes the panel and associated peripherals.
 *
 * Configures lights, timer, and I2C communication, and sets the initial system state.
 */
void Panel_Init()
{
    uint32_t light_limit = htim3.Instance->ARR;

    // Initialize lights with proper limits for even luminance
    Lights_Init(&light_red, &htim3, 4, light_limit * 0.025f);
    Lights_Init(&light_green, &htim3, 3, light_limit);

    // Start timer with interrupt
    if (HAL_TIM_Base_Start_IT(&htim3))
    {
        Error_Handler();
    }

    // Set initial states
    panelEvent = PANEL_EVENT_IDLE;
    systemEvent = SYSTEM_EVENT_IDLE;
    systemState = SYSTEM_STATE_OFF;
    systemFlag = SYSTEM_FLAG_NONE;

    timerInit(&timerPanelOnOff, 500); // set 500 tick timer for panel ONOFF triggering
}

/**
 * @brief Scans the panel for events and handles peripheral events.
 */
void Panel_Scan()
{
    Peripheral_ScanGroup();
    Panel_HandleEventPeripherals();
}

/**
 * @brief Handles peripheral events based on the system state.
 *
 * Manages light behavior and state transitions based on input events.
 */
void Panel_HandleEventPeripherals()
{

    if (timerCountUp(&timerPanelOnOff))
    {
        Panel_OnOffEnd();
    }

    if (peripheralEvent == PERIPHERAL_EVENT_ONOFF_PRESS)
    {
        Panel_OnOff();

        if (systemState == SYSTEM_STATE_OFF)
        {
            systemFlag = SYSTEM_FLAG_LIGHT_GREEN_FLASH;
        }
        // else if(systemState == SYSTEM_STATE_ON){
        //     systemFlag = SYSTEM_FLAG_LIGHT_YELLOW_FLASH;
        // }
    }
    else if (peripheralEvent == PERIPHERAL_EVENT_ONOFF_PRESS_HOLD)
    {
        if (systemState == SYSTEM_STATE_OFF)
        {
            systemFlag = SYSTEM_FLAG_LIGHT_OFF;
        }
    }

    if (peripheralEvent != PERIPHERAL_EVENT_IDLE)
    {
        panelEvent = (PanelEvent)peripheralEvent;
        if (peripheralEvent == PERIPHERAL_EVENT_ONOFF_PRESS)
        {
            if (systemState == SYSTEM_STATE_OFF)
            {
                panelEvent = (PanelEvent)PANEL_EVENT_SYSTEM_BOOT;
            }
            else if (systemState == SYSTEM_STATE_ON)
            {
                panelEvent = (PanelEvent)PANEL_EVENT_SYSTEM_SHUTDOWN;
            }
        }
    }

    // Handle panel events and execute VISCA commands
    switch (panelEvent)
    {
    case PANEL_EVENT_1_PRESS:
        overview_active = !overview_active;

        if (overview_active)
        {
            VISCA_Zoom(1, 1, 30);
            zoom_saved = zoom_current;
            zoom_current = 0;
            systemEvent = SYSTEM_EVENT_LIGHT_YELLOW_FLASH_FAST;
        }
        else
        {
            VISCA_SetZoomStep(1, zoom_saved);
            zoom_current = zoom_saved;
            systemEvent = SYSTEM_EVENT_LIGHT_OFF;
        }

        /* code */
        break;
    case PANEL_EVENT_2_PRESS:
        /* code */
        break;
    case PANEL_EVENT_3_PRESS:
        /* code */
        break;
    case PANEL_EVENT_FUNCTION_PRESS:
        /* code */
        break;
    case PANEL_EVENT_LEVEL_PRESS:
        /* code */
        break;
    case PANEL_EVENT_CAMERA_PRESS:
        /* code */
        break;
    case PANEL_EVENT_FUNCTION_CW:
        if (zoom_current < VISCA_ZOOM_STEPS)
            zoom_current++;
        VISCA_SetZoomStep(1, zoom_current);
        /* code */
        break;
    case PANEL_EVENT_FUNCTION_CCW:
        if (zoom_current > 0)
            zoom_current--;
        VISCA_SetZoomStep(1, zoom_current);
        /* code */
        break;
    case PANEL_EVENT_LEVEL_CW:
        /* code */
        break;
    case PANEL_EVENT_LEVEL_CCW:
        /* code */
        break;
    default:
        break;
    }

    systemStatePrev = systemState;

    // Handle system events and update system state
    switch (systemEvent)
    {
    case SYSTEM_EVENT_OFF:
        systemFlag = SYSTEM_FLAG_NONE;
        systemState = SYSTEM_STATE_OFF;
        break;
    case SYSTEM_EVENT_ON:
        // systemFlag = SYSTEM_FLAG_NONE;
        systemState = SYSTEM_STATE_ON;
        break;

    case SYSTEM_EVENT_LIGHT_GREEN_SOLID:
        systemFlag = SYSTEM_FLAG_LIGHT_GREEN_SOLID;
        systemState = SYSTEM_STATE_ON;
        break;
    case SYSTEM_EVENT_LIGHT_GREEN_FLASH:
        systemFlag = SYSTEM_FLAG_LIGHT_GREEN_FLASH;
        systemState = SYSTEM_STATE_ON;
        break;
    case SYSTEM_EVENT_LIGHT_GREEN_FLASH_FAST:
        systemFlag = SYSTEM_FLAG_LIGHT_GREEN_FLASH_FAST;
        systemState = SYSTEM_STATE_ON;
        break;

    case SYSTEM_EVENT_LIGHT_YELLOW_SOLID:
        systemFlag = SYSTEM_FLAG_LIGHT_YELLOW_SOLID;
        systemState = SYSTEM_STATE_ON;
        break;
    case SYSTEM_EVENT_LIGHT_YELLOW_FLASH:
        systemFlag = SYSTEM_FLAG_LIGHT_YELLOW_FLASH;
        systemState = SYSTEM_STATE_ON;
        break;
    case SYSTEM_EVENT_LIGHT_YELLOW_FLASH_FAST:
        systemFlag = SYSTEM_FLAG_LIGHT_YELLOW_FLASH_FAST;
        systemState = SYSTEM_STATE_ON;
        break;

    case SYSTEM_EVENT_LIGHT_RED_SOLID:
        systemFlag = SYSTEM_FLAG_LIGHT_RED_SOLID;
        systemState = SYSTEM_STATE_ON;
        break;
    case SYSTEM_EVENT_LIGHT_RED_FLASH:
        systemFlag = SYSTEM_FLAG_LIGHT_RED_FLASH;
        systemState = SYSTEM_STATE_ON;
        break;
    case SYSTEM_EVENT_LIGHT_RED_FLASH_FAST:
        systemFlag = SYSTEM_FLAG_LIGHT_RED_FLASH_FAST;
        systemState = SYSTEM_STATE_ON;
        break;

    case SYSTEM_EVENT_LIGHT_OFF:
        systemFlag = SYSTEM_FLAG_LIGHT_OFF;
        systemState = SYSTEM_STATE_OFF;

    default:
        break;
    }

    panelEvent = PANEL_EVENT_IDLE;
    systemEvent = SYSTEM_EVENT_IDLE;

    Panel_CheckSystemFlag();
}

/**
 * @brief Checks the current system flag and updates the light status accordingly.
 */
void Panel_CheckSystemFlag()
{
    // reset light sequence --> synchronize PWM timers
    if (systemFlagPrev != systemFlag)
    {
        Panel_LightOff(PANEL_LIGHTS_FADE_TIME_FAST);
    }

    switch (systemFlag)
    {
    case SYSTEM_FLAG_LIGHT_GREEN_SOLID:
        Panel_LightGreenSolid(PANEL_LIGHTS_FADE_TIME);
        break;
    case SYSTEM_FLAG_LIGHT_GREEN_FLASH:
        Panel_LightGreenFlash(PANEL_LIGHTS_FADE_TIME);
        break;
    case SYSTEM_FLAG_LIGHT_GREEN_FLASH_FAST:
        Panel_LightGreenFlash(PANEL_LIGHTS_FADE_TIME_FAST);
        break;

    case SYSTEM_FLAG_LIGHT_YELLOW_SOLID:
        Panel_LightYellowSolid(PANEL_LIGHTS_FADE_TIME);
        break;
    case SYSTEM_FLAG_LIGHT_YELLOW_FLASH:
        Panel_LightYellowFlash(PANEL_LIGHTS_FADE_TIME);
        break;
    case SYSTEM_FLAG_LIGHT_YELLOW_FLASH_FAST:
        Panel_LightYellowFlash(PANEL_LIGHTS_FADE_TIME_FAST);
        break;

    case SYSTEM_FLAG_LIGHT_RED_SOLID:
        Panel_LightRedSolid(PANEL_LIGHTS_FADE_TIME);
        break;
    case SYSTEM_FLAG_LIGHT_RED_FLASH:
        Panel_LightRedFlash(PANEL_LIGHTS_FADE_TIME);
        break;
    case SYSTEM_FLAG_LIGHT_RED_FLASH_FAST:
        Panel_LightRedFlash(PANEL_LIGHTS_FADE_TIME_FAST);
        break;

    case SYSTEM_FLAG_LIGHT_OFF:
        Panel_LightOff(PANEL_LIGHTS_FADE_TIME);
        break;

    default:
        break;
    }

    systemFlagPrev = systemFlag;
}

void Panel_LightGreenSolid(float fade_time)
{
    yellowSyncComplete = 0;
    Lights_FadeOut(&light_red, fade_time);
    Lights_FadeIn(&light_green, fade_time);
}

void Panel_LightGreenFlash(float fade_time)
{
    yellowSyncComplete = 0;
    Lights_FadeOut(&light_red, fade_time);
    Lights_SmoothFade(&light_green, fade_time);
}

void Panel_LightYellowSolid(float fade_time)
{
    yellowSyncComplete = 0;
    Lights_FadeIn(&light_red, fade_time);
    Lights_FadeIn(&light_green, fade_time);
}

void Panel_LightYellowFlash(float fade_time)
{
    if (!yellowSyncComplete)
    {
        uint8_t red_done = 0, green_done = 0;

        while (!red_done || !green_done)
        {
            if (!red_done)
            {
                red_done = Lights_FadeIn(&light_red, PANEL_LIGHTS_FADE_TIME_FAST);
            }
            if (!green_done)
            {
                green_done = Lights_FadeIn(&light_green, PANEL_LIGHTS_FADE_TIME_FAST);
            }
            HAL_Delay(1); // Adjust the delay as needed for your system
        }
        yellowSyncComplete = 1;
    }

    Lights_SmoothFade(&light_red, fade_time);
    Lights_SmoothFade(&light_green, fade_time);
}

void Panel_LightRedSolid(float fade_time)
{
    yellowSyncComplete = 0;
    Lights_FadeIn(&light_red, fade_time);
    Lights_FadeOut(&light_green, fade_time);
}

void Panel_LightRedFlash(float fade_time)
{
    yellowSyncComplete = 0;
    Lights_SmoothFade(&light_red, fade_time);
    Lights_FadeOut(&light_green, fade_time);
}

uint8_t Panel_LightOff(float fade_time)
{
    yellowSyncComplete = 0;
    uint8_t red_done = 0, green_done = 0;

    while (!red_done || !green_done)
    {
        if (!red_done)
        {
            red_done = Lights_FadeOut(&light_red, fade_time);
        }
        if (!green_done)
        {
            green_done = Lights_FadeOut(&light_green, fade_time);
        }
        HAL_Delay(1); // Adjust the delay as needed for your system
    }

    return 0; // Indicate completion, return value could be used for status if needed
}

/** Triggers ONOFF signal ACTIVE-LOW for 500ms, in order to boot/shutdown system */
void Panel_OnOff()
{
    Panel_OnOffBegin();
    // Panel_OnOffEnd();
}

void Panel_OnOffBegin()
{
    PanelOnOffTrigger = 1;
    timerEnable(&timerPanelOnOff);

    GPIO_InitStruct.Pin = ONOFF_Pin;

    /* Temporarily change the ONOFF pin GPIO mode to OUTPUT */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ONOFF_GPIO_Port, &GPIO_InitStruct);

    /* Trigger and hold ONOFF pin active LOW */
    HAL_GPIO_WritePin(ONOFF_GPIO_Port, ONOFF_Pin, RESET);
}

void Panel_OnOffEnd()
{
    PanelOnOffTrigger = 0;
    timerDisable(&timerPanelOnOff);

    HAL_GPIO_WritePin(ONOFF_GPIO_Port, ONOFF_Pin, SET);

    /* Change ONOFF pin GPIO mode back to INPUT */
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ONOFF_GPIO_Port, &GPIO_InitStruct);
}
