/**
 * IMP Project 2018
 * Utils functions
 * @brief Utils functions
 * @author Matej Mitas, xmitas02
 * @file imp_util.c
 */

#include "IMP_util.h"

/**
 * @brief Simple state toggle for program select
 * @return next state
 */
int IMG_FsmStateHandler(void) {
	if (status < 3) {
		status += 1;
	} else {
		status = 0;
	}
	return status;
}

/**
 * @brief Helper function for resetting LEDs
 */
void IMP_ResetLeds(void) {
	LED_RED_OFF();
	LED_GREEN_OFF();
	LED_BLUE_OFF();
}

/**
 * @brief Interrupt handler for select button (SW1)
 */
void BOARD_SW1_IRQ_HANDLER(void) {
	GPIO_PortClearInterruptFlags(BOARD_SW1_GPIO, 1U << BOARD_SW1_GPIO_PIN);
	IMP_ResetLeds();
	/* Simple FSM implementation for handling program states */
	switch (IMG_FsmStateHandler()) {
		case FSM_MODE_FREEFALL:
			LED_RED_ON();
			printf("Mode: FREEFALL\n");
			break;
		case FSM_MODE_TILT:
			LED_GREEN_ON();
			printf("Mode: TILT\n");
			break;
		case FSM_PEDOMETER:
			LED_BLUE_ON();
			printf("Mode: PEDOMETER\n");
			break;
		case FSM_SPEED:
			LED_GREEN_ON();
			LED_BLUE_ON();
			printf("Mode: VELOCITY\n");
			break;
	}
}

/**
 * @brief Interrupt handler for master reset button (SW3)
 */
void BOARD_SW3_IRQ_HANDLER(void) {
	GPIO_PortClearInterruptFlags(BOARD_SW3_GPIO, 1U << BOARD_SW3_GPIO_PIN);
	status = FSM_MODE_START;
	IMP_ResetLeds();
}

/**
 * @brief Enabling hardware interrupts for both buttons
 */
void IMP_EnableButtonIrq(void) {
    PORT_SetPinInterruptConfig(BOARD_SW1_PORT, BOARD_SW1_GPIO_PIN, kPORT_InterruptFallingEdge);
    EnableIRQ(BOARD_SW1_IRQ);
    PORT_SetPinInterruptConfig(BOARD_SW3_PORT, BOARD_SW3_GPIO_PIN, kPORT_InterruptFallingEdge);
    EnableIRQ(BOARD_SW3_IRQ);
}

/**
 * @brief Bridge function between business logic and data fetching
 * @param x X-axis data
 * @param y Y-axis data
 * @param z Z-axis data
 */
void IMP_BindAccData(int16_t x, int16_t y, int16_t z) {
	switch (status) {
		case FSM_MODE_FREEFALL:
			IMP_Freefall(x,y,z);
			break;
		case FSM_MODE_TILT:
			IMP_Tilt(x,y,z);
			break;
		case FSM_PEDOMETER:
			IMP_Pedometer(z);
			break;
		case FSM_SPEED:
			IMP_Speed(x);
			break;
	}
}
