/**
 * IMP Project 2018
 * Header for communication
 * @brief Header for communication
 * @author Matej Mitas, xmitas02
 * @file imp_com.h
 */

#ifndef IMP_COM_H_
#define IMP_COM_H_

/* Libs */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <cmsis_gcc.h>
/* SDK files */
#include "board.h"
#include "fsl_i2c.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "pin_mux.h"
/* Project files*/
#include "imp_util.h"
#include "imp_logic.h"
/* Defines - Accelerometer */
#define ACCEL_I2C_CLK_SRC I2C1_CLK_SRC
#define ACCEL_I2C_CLK_FREQ CLOCK_GetFreq(I2C1_CLK_SRC)
#define ACCEL_STATUS 0x00U
#define ACCEL_XYZ_DATA_CFG 0x0EU
#define ACCEL_CTRL_REG1 0x2AU
#define ACCEL_WHOAMI_REG 0x0DU
#define ACCEL_READ_TIMES 65535U
/* Defines - I2C */
#define I2C_BAUDRATE 100000U
#define I2C_RELEASE_BUS_COUNT 100U
#define I2C_RELEASE_SDA_PORT PORTD
#define I2C_RELEASE_SCL_PORT PORTD
#define I2C_RELEASE_SDA_GPIO GPIOD
#define I2C_RELEASE_SDA_PIN 6U
#define I2C_RELEASE_SCL_GPIO GPIOD
#define I2C_RELEASE_SCL_PIN 7U
#define I2C_ADDR 0x1D
/* Exposed functions */
void IMP_ComInit(void);

#endif /* IMP_COM_H_ */
