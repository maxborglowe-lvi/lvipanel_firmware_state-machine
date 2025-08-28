
#include "peripherals.h"
#include "lights.h"
#include "tim.h"
// #include "i2c.h"
#include "panel_events.h"
#include "timer.h"

#define PANEL_LIGHTS_FADE_TIME 0.5f
#define PANEL_LIGHTS_FADE_TIME_FAST 5.0f

#define TRY_BOOT_CYCLES 10000

typedef enum
{
    SYSTEM_STATE_OFF = 1,
    SYSTEM_STATE_BOOT,
    SYSTEM_STATE_ON,
    SYSTEM_STATE_SHUTDOWN

} SystemState;

typedef enum
{
    SYSTEM_FLAG_NONE,
    SYSTEM_FLAG_LIGHT_GREEN_SOLID,
    SYSTEM_FLAG_LIGHT_GREEN_FLASH,
    SYSTEM_FLAG_LIGHT_GREEN_FLASH_FAST,
    SYSTEM_FLAG_LIGHT_YELLOW_SOLID,
    SYSTEM_FLAG_LIGHT_YELLOW_FLASH,
    SYSTEM_FLAG_LIGHT_YELLOW_FLASH_FAST,
    SYSTEM_FLAG_LIGHT_RED_SOLID,
    SYSTEM_FLAG_LIGHT_RED_FLASH,
    SYSTEM_FLAG_LIGHT_RED_FLASH_FAST,
    SYSTEM_FLAG_LIGHT_OFF,
} SystemFlag;

void Panel_Init();
void Panel_Scan();

void Panel_HandleEventPeripherals();
void Panel_OnOff();
void Panel_OnOffBegin();
void Panel_OnOffEnd();
void Panel_CheckSystemFlag();
void Panel_TryBoot();

void Panel_LightGreenSolid(float fade_speed);
void Panel_LightGreenFlash(float fade_speed);

void Panel_LightYellowSolid(float fade_speed);
void Panel_LightYellowFlash(float fade_speed);

void Panel_LightRedSolid(float fade_speed);
void Panel_LightRedFlash(float fade_speed);

uint8_t Panel_LightOff(float fade_speed);