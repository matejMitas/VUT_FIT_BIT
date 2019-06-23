/**
 * IMP Project 2018
 * Main file
 * @brief Main file
 * @author Matej Mitas, xmitas02
 * @file imp_xmitas02.c
 */

#include "IMP_xmitas02.h"

/*
 * @brief   Application entry point.
 */
int main(void) {
	/* Generic settings for init */
     BOARD_InitPins();
    BOARD_InitLEDs();
    BOARD_InitButtons();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    /* Button interrupt request enable */
    IMP_EnableButtonIrq();
    /* Init communication over I2C */
    IMP_ComInit();
    /* For the duration of the program */
    while (1) {
    }
}
