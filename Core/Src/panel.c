#include "panel.h"

Light light_red;
Light light_green;

Timer timer_main;

PanelEvent panelEvent;
SystemEvent systemEvent;
SystemState systemState;


void Panel_Init(){
    Lights_Init(&light_red, &htim3, 4);
    Lights_Init(&light_green, &htim3, 3);

    if (HAL_TIM_Base_Start_IT(&htim3))
	{
		Error_Handler();
	}

    i2c_rx = (uint8_t*)&systemEvent;
    i2c_tx = (uint8_t*)&panelEvent;

    panelEvent = PANEL_EVENT_IDLE;
    systemEvent = SYSTEM_EVENT_IDLE;
    systemState = SYSTEM_STATE_OFF;

}

void Panel_Scan(){

    Peripheral_ScanGroup();
    Panel_HandleEventPeripherals();

}

void Panel_HandleEventPeripherals() {
    // Allow only ONOFF_PRESS event when the system is OFF
    if (systemState == SYSTEM_STATE_OFF) {
        if (peripheralEvent == PERIPHERAL_EVENT_ONOFF_PRESS) {
            panelEvent = PANEL_EVENT_SYSTEM_BOOT;
            systemState = SYSTEM_STATE_BOOT;
        }
        // Ignore all other events when system is OFF
        return;
    }

    // Handle peripheral events normally when the system is ON
    if (systemState == SYSTEM_STATE_ON) {
        if (peripheralEvent != PERIPHERAL_EVENT_IDLE) {
            panelEvent = (PanelEvent)peripheralEvent;
        }

        switch (peripheralEvent) {
            case PERIPHERAL_EVENT_ONOFF_PRESS:
                panelEvent = PANEL_EVENT_SYSTEM_SHUTDOWN;
                systemState = SYSTEM_STATE_SHUTDOWN;
                break;
            case PERIPHERAL_EVENT_ONOFF_PRESS_DOUBLE:
                // Add handling if needed
                break;
            case PERIPHERAL_EVENT_ONOFF_PRESS_HOLD:
                // Add handling if needed
                break;
            case PERIPHERAL_EVENT_ZOOM_PRESS:
                // Add handling if needed
                break;
            case PERIPHERAL_EVENT_ZOOM_PRESS_DOUBLE:
                // Add handling if needed
                break;
            case PERIPHERAL_EVENT_ZOOM_PRESS_HOLD:
                // Add handling if needed
                break;
            case PERIPHERAL_EVENT_FUNCTION_PRESS:
                // Add handling if needed
                break;
            case PERIPHERAL_EVENT_FUNCTION_PRESS_DOUBLE:
                // Add handling if needed
                break;
            case PERIPHERAL_EVENT_FUNCTION_PRESS_HOLD:
                // Add handling if needed
                break;
            case PERIPHERAL_EVENT_COLOR_ARTIFICIAL_PRESS:
                // Add handling if needed
                break;
            case PERIPHERAL_EVENT_COLOR_ARTIFICIAL_PRESS_DOUBLE:
                // Add handling if needed
                break;
            case PERIPHERAL_EVENT_COLOR_ARTIFICIAL_PRESS_HOLD:
                // Add handling if needed
                break;
            case PERIPHERAL_EVENT_COLOR_NATURAL_PRESS:
                // Add handling if needed
                break;
            case PERIPHERAL_EVENT_COLOR_NATURAL_PRESS_DOUBLE:
                // Add handling if needed
                break;
            case PERIPHERAL_EVENT_COLOR_NATURAL_PRESS_HOLD:
                // Add handling if needed
                break;
            case PERIPHERAL_EVENT_ZOOM_UP:
                Lights_LinearIncrease(&light_green);
                break;
            case PERIPHERAL_EVENT_ZOOM_DN:
                Lights_LinearDecrease(&light_green);
                break;
            case PERIPHERAL_EVENT_FUNCTION_UP:
                Lights_LinearIncrease(&light_red);
                break;
            case PERIPHERAL_EVENT_FUNCTION_DN:
                Lights_LinearDecrease(&light_red);
                break;
            default:
                break;
        }
    }

    // Handle system events and update system state
    switch (systemEvent) {
        case SYSTEM_EVENT_OFF:
            if (systemState == SYSTEM_STATE_SHUTDOWN) {
                systemState = SYSTEM_STATE_OFF;
            }
            break;
        case SYSTEM_EVENT_ON:
            if (systemState == SYSTEM_STATE_BOOT) {
                systemState = SYSTEM_STATE_ON;
            }
            break;
        default:
            break;
    }

    systemEvent = SYSTEM_EVENT_IDLE;
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
    memset(i2c_tx, 0, PACKET_SIZE);
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *I2cHandle){
    // memset(i2c_rx, 0, PACKET_SIZE);
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{

    if(TransferDirection == I2C_DIRECTION_TRANSMIT){
        if(HAL_I2C_Slave_Seq_Receive_IT(hi2c, (uint8_t*)i2c_rx, PACKET_SIZE, I2C_FIRST_AND_LAST_FRAME) != HAL_OK){
            Error_Handler();
        }
    } else if(TransferDirection == I2C_DIRECTION_RECEIVE){
        if(HAL_I2C_Slave_Seq_Transmit_IT(hi2c, (uint8_t*)i2c_tx, PACKET_SIZE, I2C_FIRST_AND_LAST_FRAME) != HAL_OK){
            Error_Handler();
        }
    }
    Xfer_Complete = 1;
}

extern void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if(HAL_I2C_EnableListen_IT(&hi2c1) != HAL_OK){
        Error_Handler();
    }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2cHandle)
{
    if(HAL_I2C_EnableListen_IT(&hi2c1) != HAL_OK){
        Error_Handler();
    }
  /** Error_Handler() function is called when error occurs.
    * 1- When Slave doesn't acknowledge its address, Master restarts communication.
    * 2- When Master doesn't acknowledge the last data transferred, Slave doesn't care in this example.
    */
    if (HAL_I2C_GetError(I2cHandle) != HAL_I2C_ERROR_AF)
    {
        Error_Handler();
    }
}