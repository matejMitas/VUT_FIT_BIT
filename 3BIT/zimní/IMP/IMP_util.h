/**
 * IMP Project 2018
 * Header for utils functions
 * @brief Header for utils functions
 * @author Matej Mitas, xmitas02
 * @file imp_util.h
 */

#ifndef IMP_UTIL_H_
#define IMP_UTIL_H_

/* Libs */
#include <stdio.h>
#include <math.h>
#include "board.h"
#include "peripherals.h"
#include "clock_config.h"
#include "MKL27Z644.h"
/* Defines - FSM */
#define FSM_MODE_START -1
#define FSM_MODE_FREEFALL 0
#define	FSM_MODE_TILT 1
#define FSM_PEDOMETER 2
#define FSM_SPEED 3
/* Variables */
volatile static int status = FSM_MODE_START;
/* Exposed functions */
void IMP_EnableButtonIrq(void);
void IMP_BindAccData(int16_t x, int16_t y, int16_t z);

#endif /* IMP_UTIL_H_ */
