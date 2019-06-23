/**
 * IMP Project 2018
 * Header for business logic
 * @brief Header for business logic
 * @author Matej Mitas, xmitas02
 * @file imp_logic.h
 */

#ifndef IMP_LOGIC_H_
#define IMP_LOGIC_H_

/* Libs */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
/* Defines - Accelerometer */
#define ACC_RESOLUTION 2048
/* Defines - Freefall */
/* Set threshold as +-0.2g */
#define ACC_FREEFALL_THRESHOLD_DOWN -409
#define ACC_FREEFALL_THRESHOLD_UP 409
/* Defines - Tilt */
#define ACC_TILT_FULL_CIRCLE 360
#define ACC_TILT_HALF_CIRCLE 180
#define ACC_TILT_QUATER_CIRCLE 90
/* Defines - Pedometer */
#define ACC_PEDOMETER_LOW_THRESHOLD 0
#define ACC_PEDOMETER_HIGH_THRESHOLD 4000
#define ACC_PEDOMETER_STILL 0
#define ACC_RISING_UP 1
#define ACC_RISING_DOWN 2
/* Defines - Velocity */
#define ACC_VELOCITY_CALIB_SAMPLES 16
#define ACC_VELOCITY_IDLE_THRESHOLD 50
#define ACC_VELOCITY_FLOAT_NORM 1000
/* Variables - Pedometer*/
volatile static uint8_t pedometer_status = ACC_PEDOMETER_STILL;
/* Variables - Velocity*/
volatile static int16_t prev_accelaration = 0;
volatile static int16_t cur_accelaration = 0;
volatile static int16_t cur_velocity = 0;
volatile static int16_t prev_velocity = 0;
volatile static uint8_t sample_counter = 0;
volatile static int16_t sample_x = 0;
/* Exposed functions */
void IMP_PrintFloat(int16_t data);
void IMP_Freefall(int16_t x, int16_t y, int16_t z);
void IMP_Tilt(int16_t x, int16_t y, int16_t z);
void IMP_Pedometer(int16_t z);
void IMP_Speed(int16_t x);

#endif /* IMP_LOGIC_H_ */
