#ifndef __VISCA_H
#define __VISCA_H

#include "stm32f0xx_hal.h" // replace with your series header

void VISCA_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef VISCA_Send(uint8_t cam_id, const uint8_t *cmd, uint8_t len);

void VISCA_CameraPower(uint8_t cam_id, uint8_t on);
void VISCA_PanTiltHome(uint8_t cam_id);
void VISCA_ZoomTele(uint8_t cam_id);
void VISCA_ZoomWide(uint8_t cam_id);
void VISCA_DisplayTitle(uint8_t cam_id, const char *text);

/* Zoom */
void VISCA_ZoomTeleVar(uint8_t cam_id);
void VISCA_ZoomWideVar(uint8_t cam_id);
void VISCA_ZoomStop(uint8_t cam_id);

/* Preset */
void VISCA_SetPreset(uint8_t cam_id, uint8_t preset_id);
void VISCA_RecallPreset(uint8_t cam_id, uint8_t preset_id);

/* Autofocus */
void VISCA_AutoFocusOn(uint8_t cam_id);
void VISCA_AutoFocusOff(uint8_t cam_id);

/* Digital Zoom */
void VISCA_DigitalZoomOn(uint8_t cam_id);
void VISCA_DigitalZoomOff(uint8_t cam_id);

/* Focus */
void VISCA_FocusNear(uint8_t cam_id);
void VISCA_FocusFar(uint8_t cam_id);
void VISCA_FocusStop(uint8_t cam_id);

/* Exposure / Brightness */
void VISCA_ExposureAuto(uint8_t cam_id);
void VISCA_BrightnessMode(uint8_t cam_id);
void VISCA_BrightnessUp(uint8_t cam_id);
void VISCA_BrightnessDown(uint8_t cam_id);

#define VISCA_ZOOM_STEPS 32

void VISCA_Zoom(uint8_t cam_id, int direction, uint8_t speed);
void VISCA_SetZoomStep(uint8_t cam_id, uint8_t step);

#endif
