#include "visca.h"

static UART_HandleTypeDef *visca_huart;

void VISCA_Init(UART_HandleTypeDef *huart)
{
    visca_huart = huart;
}

// Core send function
HAL_StatusTypeDef VISCA_Send(uint8_t cam_id, const uint8_t *cmd, uint8_t len)
{
    uint8_t buf[16]; // enough for most VISCA commands
    uint8_t idx = 0;

    buf[idx++] = 0x80 | (cam_id & 0x0F); // Destination address
    for (uint8_t i = 0; i < len; i++)
    {
        buf[idx++] = cmd[i];
    }
    buf[idx++] = 0xFF; // End of message

    return HAL_UART_Transmit(visca_huart, buf, idx, HAL_MAX_DELAY);
}

/* ====== Helpers ====== */

// Power ON/OFF
void VISCA_CameraPower(uint8_t cam_id, uint8_t on)
{
    uint8_t cmd[] = {0x01, 0x04, 0x00, on ? 0x02 : 0x03};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

// Pan-Tilt Home
void VISCA_PanTiltHome(uint8_t cam_id)
{
    uint8_t cmd[] = {0x01, 0x06, 0x04};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

// Zoom Tele (in)
void VISCA_ZoomTele(uint8_t cam_id)
{
    uint8_t cmd[] = {0x01, 0x04, 0x07, 0x02};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

// Zoom Wide (out)
void VISCA_ZoomWide(uint8_t cam_id)
{
    uint8_t cmd[] = {0x01, 0x04, 0x07, 0x03};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

/* ====== Zoom controls ====== */
void VISCA_ZoomTeleVar(uint8_t cam_id)
{
    uint8_t cmd[] = {0x01, 0x04, 0x07, 0x35};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

void VISCA_ZoomWideVar(uint8_t cam_id)
{
    uint8_t cmd[] = {0x01, 0x04, 0x07, 0x25};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

void VISCA_ZoomStop(uint8_t cam_id)
{
    uint8_t cmd[] = {0x01, 0x04, 0x07, 0x00};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

/* ====== Preset controls ====== */
void VISCA_SetPreset(uint8_t cam_id, uint8_t preset_id)
{
    uint8_t cmd[] = {0x01, 0x04, 0x3F, 0x01, preset_id};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

void VISCA_RecallPreset(uint8_t cam_id, uint8_t preset_id)
{
    uint8_t cmd[] = {0x01, 0x04, 0x3F, 0x02, preset_id};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

/* ====== Autofocus controls ====== */
void VISCA_AutoFocusOn(uint8_t cam_id)
{
    uint8_t cmd[] = {0x01, 0x04, 0x38, 0x02};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

void VISCA_AutoFocusOff(uint8_t cam_id)
{
    uint8_t cmd[] = {0x01, 0x04, 0x38, 0x03};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

/* ====== Digital zoom controls ====== */
void VISCA_DigitalZoomOn(uint8_t cam_id)
{
    uint8_t cmd[] = {0x01, 0x04, 0x06, 0x02};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

void VISCA_DigitalZoomOff(uint8_t cam_id)
{
    uint8_t cmd[] = {0x01, 0x04, 0x06, 0x03};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

/* ====== Focus controls ====== */
void VISCA_FocusNear(uint8_t cam_id)
{
    uint8_t cmd[] = {0x01, 0x04, 0x08, 0x25};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

void VISCA_FocusFar(uint8_t cam_id)
{
    uint8_t cmd[] = {0x01, 0x04, 0x08, 0x35};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

void VISCA_FocusStop(uint8_t cam_id)
{
    uint8_t cmd[] = {0x01, 0x04, 0x08, 0x00};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

/* ====== Exposure / Brightness controls ====== */
void VISCA_ExposureAuto(uint8_t cam_id)
{
    uint8_t cmd[] = {0x01, 0x04, 0x39, 0x00};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

void VISCA_BrightnessMode(uint8_t cam_id)
{
    uint8_t cmd[] = {0x01, 0x04, 0x39, 0x0D};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

void VISCA_BrightnessUp(uint8_t cam_id)
{
    uint8_t cmd[] = {0x01, 0x04, 0x0D, 0x02};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

void VISCA_BrightnessDown(uint8_t cam_id)
{
    uint8_t cmd[] = {0x01, 0x04, 0x0D, 0x03};
    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

void VISCA_Zoom(uint8_t cam_id, int direction, uint8_t speed)
{
    if (speed > 7)
        speed = 7; // clamp

    uint8_t cmd[] = {0x01, 0x04, 0x07, 0x00};

    if (direction > 0)
    {
        cmd[3] = 0x30 | speed; // Tele
    }
    else if (direction < 0)
    {
        cmd[3] = 0x20 | speed; // Wide
    }
    else
    {
        cmd[3] = 0x00; // Stop
    }

    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

void VISCA_SetZoomStep(uint8_t cam_id, uint8_t step)
{
    uint8_t step_max = VISCA_ZOOM_STEPS - 1;

    if (step > step_max)
        step = step_max; // clamp

    uint16_t zoom_pos = (step * 0x3FFF) / step_max;

    uint8_t cmd[] = {
        0x01, 0x04, 0x47,
        (uint8_t)((zoom_pos >> 12) & 0x0F), // ZP_H
        (uint8_t)((zoom_pos >> 8) & 0x0F),  // ZP_M
        (uint8_t)((zoom_pos >> 4) & 0x0F),  // ZP_L
        (uint8_t)(zoom_pos & 0x0F)          // ZP_LL
    };

    VISCA_Send(cam_id, cmd, sizeof(cmd));
}

// void VISCA_DisplayTitle(uint8_t cam_id, const char *text)
// {
//     uint8_t buf[20];
//     uint8_t len = 0;

//     buf[len++] = 0x01;
//     buf[len++] = 0x04;
//     buf[len++] = 0x72;

//     // Copy characters (VISCA supports limited range: space 0x20 – ‘_’ 0x5F)
//     while (*text && len < sizeof(buf))
//     {
//         uint8_t c = *text++;
//         if (c < 0x20 || c > 0x5F)
//             c = '_'; // clamp unsupported
//         buf[len++] = c & 0x7F;
//     }

//     VISCA_Send(cam_id, buf, len);
// }