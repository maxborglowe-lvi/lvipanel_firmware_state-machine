#include "peripherals.h"

uint8_t peripheral_amount_btns = MAX_AMT_BTNS;
uint8_t peripheral_amount_encs = MAX_AMT_ENCS;

PeripheralGroup *peripheralGroup;

PeripheralEvent peripheralEvent;

// Use a lookup table to handle button presses
static PeripheralEvent buttonEventTable[MAX_AMT_BTNS][3] = {
	// BTN_STATE_PRESS           BTN_STATE_HOLD           BTN_STATE_REL
	[BTN_ID_ONOFF]				= { PERIPHERAL_EVENT_ONOFF_PRESS, PERIPHERAL_EVENT_ONOFF_PRESS_HOLD, PERIPHERAL_EVENT_ONOFF_PRESS_DOUBLE },
	[BTN_ID_FUNCTION] 			= { PERIPHERAL_EVENT_FUNCTION_PRESS, PERIPHERAL_EVENT_FUNCTION_PRESS_HOLD, PERIPHERAL_EVENT_FUNCTION_PRESS_DOUBLE },
	[BTN_ID_ZOOM]     			= { PERIPHERAL_EVENT_ZOOM_PRESS,     PERIPHERAL_EVENT_ZOOM_PRESS_HOLD,     PERIPHERAL_EVENT_ZOOM_PRESS_DOUBLE },
	[BTN_ID_COLOR_ARTIFICIAL] 	= { PERIPHERAL_EVENT_COLOR_ARTIFICIAL_PRESS, PERIPHERAL_EVENT_COLOR_ARTIFICIAL_PRESS_HOLD, PERIPHERAL_EVENT_COLOR_ARTIFICIAL_PRESS_DOUBLE },
	[BTN_ID_COLOR_NATURAL]    	= { PERIPHERAL_EVENT_COLOR_NATURAL_PRESS,    PERIPHERAL_EVENT_COLOR_NATURAL_PRESS_HOLD,    PERIPHERAL_EVENT_COLOR_NATURAL_PRESS_DOUBLE }
};

static PeripheralEvent encoderEventTable[MAX_AMT_ENCS][2] = {
	[ENC_ID_FUNCTION] = { PERIPHERAL_EVENT_FUNCTION_UP, PERIPHERAL_EVENT_FUNCTION_DN },
	[ENC_ID_ZOOM]     = { PERIPHERAL_EVENT_ZOOM_UP,     PERIPHERAL_EVENT_ZOOM_DN }
};

uint8_t enc_read;

/**
 * @brief Initializes button.
 * @param btn The button to be initialized.
 * @param id The unique identifier of the button.
 * @param mode The mode of the button.
 * @param PORT The GPIO port of the button.
 * @param PIN The GPIO pin of the button.
 */
void Peripheral_ButtonInit(Button *btn, PeripheralID id, ButtonMode mode, GPIO_TypeDef *PORT, uint16_t PIN) {
    btn->id = id;
    btn->mode = mode;
    btn->sig = (mode == BTN_MODE_LO) ? BTN_LO_RST : BTN_HI_RST;
    btn->state = BTN_STATE_RST;
    btn->init_flag = PERIPHERAL_INIT;
    btn->PORT = PORT;
    btn->PIN = PIN;
    timerInit(&btn->timer_hold, PERIPHERAL_HOLD_TIME);
    timerInit(&btn->timer_press, PERIPHERAL_HOLD_TIME);
    timerInit(&btn->timer_press_double, PERIPHERAL_PRESS_DOUBLE_TIME);
    timerDisable(&btn->timer_hold);
    timerDisable(&btn->timer_press);
    timerDisable(&btn->timer_press_double);
}

/**
 * @brief Scans the current value of a button and updates its state.
 * @param btn The button being scanned.
 */
void Peripheral_ButtonScan(Button *btn) {
    uint8_t btn_sig_prev = btn->sig;
    btn->sig = HAL_GPIO_ReadPin(btn->PORT, btn->PIN);
    uint8_t btn_read = CONCAT_BTN_READ(btn_sig_prev, btn->sig);

    if (btn->mode == BTN_MODE_LO) {
        btn->state = (btn_read == BTN_LO_PRESS) ? BTN_STATE_PRESS :
                     (btn_read == BTN_LO_HOLD) ? BTN_STATE_HOLD :
                     (btn_read == BTN_LO_REL) ? BTN_STATE_REL :
                     (btn_read == BTN_LO_RST) ? BTN_STATE_RST : btn->state;
    } else {
        btn->state = (btn_read == BTN_HI_PRESS) ? BTN_STATE_PRESS :
                     (btn_read == BTN_HI_HOLD) ? BTN_STATE_HOLD :
                     (btn_read == BTN_HI_REL) ? BTN_STATE_REL :
                     (btn_read == BTN_HI_RST) ? BTN_STATE_RST : btn->state;
    }
}

/** @brief Executes the scanned button commands. */
void Peripheral_ButtonExec(Button *btn) {
    uint8_t notPressDouble = timerCountUp(&btn->timer_press_double);
    peripheralEvent = PERIPHERAL_EVENT_IDLE;

    if (notPressDouble) {
        timerDisable(&btn->timer_press_double);
    }

    

    switch (btn->state) {
        case BTN_STATE_PRESS:
            timerReset(&btn->timer_hold);
            timerEnable(&btn->timer_hold);
            peripheralEvent = buttonEventTable[btn->id][PERIPHERAL_EVENT_ID_PRESS];
            break;
        case BTN_STATE_HOLD:
            if (timerCountUp(&btn->timer_hold)) {
                peripheralEvent = buttonEventTable[btn->id][PERIPHERAL_EVENT_ID_PRESS_HOLD];
            }
            break;
        case BTN_STATE_REL:
            if (timerIsEnabled(&btn->timer_press_double)) {
                peripheralEvent = buttonEventTable[btn->id][PERIPHERAL_EVENT_ID_PRESS_DOUBLE];
                timerDisable(&btn->timer_press_double);
            } else {
                timerReset(&btn->timer_press_double);
                timerEnable(&btn->timer_press_double);
            }
            timerDisable(&btn->timer_hold);
            break;
        default:
            break;
    }
}

/**
 * @brief Initializes encoder.
 * @param enc The encoder to be initialized.
 * @param id The unique identifier of the encoder.
 * @param mode The mode of the encoder.
 * @param PORT_A The GPIO port for encoder channel A.
 * @param PIN_A The GPIO pin for encoder channel A.
 * @param PORT_B The GPIO port for encoder channel B.
 * @param PIN_B The GPIO pin for encoder channel B.
 */
void Peripheral_EncoderInit(Encoder *enc, PeripheralID id, EncoderMode mode, GPIO_TypeDef *PORT_A,
                             uint16_t PIN_A, GPIO_TypeDef *PORT_B, uint16_t PIN_B) {
    enc->id = id;
    enc->mode = mode;
    enc->state = ENC_STATE_RST;
    enc->init_flag = PERIPHERAL_INIT;
    enc->ch_a = 0x00;
    enc->ch_b = 0x00;
    enc->PORT_A = PORT_A;
    enc->PORT_B = PORT_B;
    enc->PIN_A = PIN_A;
    enc->PIN_B = PIN_B;
}

/**
 * @brief Scans the current values of an encoder's A- and B-channels and updates its state.
 * @param enc The encoder being scanned.
 */
void Peripheral_EncoderScan(Encoder *enc) {
    if (enc->state == ENC_STATE_RST) {
        uint8_t enc_ch_a_prev = enc->ch_a;
        uint8_t enc_ch_b_prev = enc->ch_b;

        enc->ch_a = HAL_GPIO_ReadPin(enc->PORT_A, enc->PIN_A);
        enc->ch_b = HAL_GPIO_ReadPin(enc->PORT_B, enc->PIN_B);

        enc_read = CONCAT_ENC_READ(enc_ch_a_prev, enc->ch_a, enc_ch_b_prev, enc->ch_b);

        if (enc->mode == ENC_MODE_UNMATCHED || enc->mode == ENC_MODE_MATCHED) {
            enc->state = (enc_read == ENC_CW1 || enc_read == ENC_CW2) ? ENC_STATE_CW :
                         (enc_read == ENC_CCW1 || enc_read == ENC_CCW2) ? ENC_STATE_CCW :
                         (enc_read == ENC_RST1 || enc_read == ENC_RST2) ? ENC_STATE_RST : enc->state;
        }
    }
}

/**
 * @brief Executes the scanned encoder commands.
 * @param enc The encoder being executed.
 */
void Peripheral_EncoderExec(Encoder *enc) {
    peripheralEvent = (enc->state == ENC_STATE_CW) ? encoderEventTable[enc->id][0] :
                      (enc->state == ENC_STATE_CCW) ? encoderEventTable[enc->id][1] : PERIPHERAL_EVENT_IDLE;
    enc->state = ENC_STATE_RST;
}

void Peripheral_InitGroup(void){

	// Allocate memory for the peripheral group
	peripheralGroup = (PeripheralGroup *)malloc(sizeof(PeripheralGroup));
	if (peripheralGroup == NULL) {
		// Handle memory allocation failure
		return;
	}

	memset(peripheralGroup, 0, sizeof(PeripheralGroup)); // Zero-initialize the memory

	Peripheral_ButtonInit(&peripheralGroup->btn[0], BTN_ID_ONOFF, BTN_MODE_LO, ONOFF_GPIO_Port, ONOFF_Pin);
	Peripheral_ButtonInit(&peripheralGroup->btn[1], BTN_ID_FUNCTION, BTN_MODE_LO, BTN_FUNCTION_GPIO_Port, BTN_FUNCTION_Pin);
	Peripheral_ButtonInit(&peripheralGroup->btn[2], BTN_ID_ZOOM, BTN_MODE_LO, BTN_ZOOM_GPIO_Port, BTN_ZOOM_Pin);
	Peripheral_ButtonInit(&peripheralGroup->btn[3], BTN_ID_COLOR_NATURAL, BTN_MODE_LO, BTN_COLOR_NATURAL_GPIO_Port, BTN_COLOR_NATURAL_Pin);
	Peripheral_ButtonInit(&peripheralGroup->btn[4], BTN_ID_COLOR_ARTIFICIAL, BTN_MODE_LO, BTN_COLOR_ARTIFICIAL_GPIO_Port, BTN_COLOR_ARTIFICIAL_Pin);

	Peripheral_EncoderInit(&peripheralGroup->enc[0], ENC_ID_FUNCTION, ENC_MODE_UNMATCHED, ENC_FUNCTION_CH_A_GPIO_Port, ENC_FUNCTION_CH_A_Pin, ENC_FUNCTION_CH_B_GPIO_Port, ENC_FUNCTION_CH_B_Pin);
	Peripheral_EncoderInit(&peripheralGroup->enc[1], ENC_ID_ZOOM, ENC_MODE_UNMATCHED, ENC_ZOOM_CH_A_GPIO_Port, ENC_ZOOM_CH_A_Pin, ENC_ZOOM_CH_B_GPIO_Port, ENC_ZOOM_CH_B_Pin);

	peripheral_amount_btns = 5;
	peripheral_amount_encs = 2;
}

void Peripheral_ScanGroup(void){
	/* Scan buttons active in panel */
	for (int i = 0; i < peripheral_amount_btns; i++) {
		/* Perform scan and exec if button is initialized*/
		if(peripheralGroup->btn[i].init_flag){
			Peripheral_ButtonScan(&peripheralGroup->btn[i]);
			/* Execute commands gathered during scan */
			Peripheral_ButtonExec(&peripheralGroup->btn[i]);
			if(peripheralEvent != PERIPHERAL_EVENT_IDLE) return;
		}
	}

	/* Scan encoders active in panel */
	for (int i = 0; i < peripheral_amount_encs; i++) {
		/* Perform scan and exec if encoder is initialized*/
		if(peripheralGroup->enc[i].init_flag){
			Peripheral_EncoderScan(&peripheralGroup->enc[i]);
			/* Execute commands gathered during scan */
			Peripheral_EncoderExec(&peripheralGroup->enc[i]);
			if(peripheralEvent != PERIPHERAL_EVENT_IDLE) return;
		}
	}
}

