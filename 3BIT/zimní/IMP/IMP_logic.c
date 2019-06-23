/**
 * IMP Project 2018
 * Business logic
 * @brief Business logic
 * @author Matej Mitas, xmitas02
 * @file imp_logic.c
 */

#include "IMP_logic.h"

/**
 * @brief Prints out float in integer form
 * @param data integer representation
 */
void IMP_PrintFloat(int16_t data) {
	uint8_t sign = ' ';
	if (data/1000 < 0) {
		sign = '-';
	}
	printf("%c%1d.%1d%1d%1d m/s\n", sign, abs(data/1000), abs((data/100)%10), abs((data/10)%10), abs(data%10));
}

/**
 * @brief Detecting freefall
 * @param x X-axis
 * @param y Y-axis
 * @param z Z-axis
 */
void IMP_Freefall(int16_t x, int16_t y, int16_t z) {
	/* Detecting free fall according to recommeded thresholds */
	if ((x > ACC_FREEFALL_THRESHOLD_DOWN && x < ACC_FREEFALL_THRESHOLD_UP) &&
		(y > ACC_FREEFALL_THRESHOLD_DOWN && y < ACC_FREEFALL_THRESHOLD_UP) &&
		(z > ACC_FREEFALL_THRESHOLD_DOWN && z < ACC_FREEFALL_THRESHOLD_UP)) {
		printf("Freefall detected\n");
	}
}

/**
 * @brief Detecting tilt
 * @param x X-axis
 * @param y Y-axis
 * @param z Z-axis
 */
void IMP_Tilt(int16_t x, int16_t y, int16_t z) {
	/* Normalisation angles on step circle
	 * All oriented by Z-Axis, each quater is normalized
	 */
	if (z > 0) {
		if (x < 0) {
			x = ACC_TILT_FULL_CIRCLE - (x * ACC_TILT_QUATER_CIRCLE / ACC_RESOLUTION * -1);
		} else {
			x = x* ACC_TILT_QUATER_CIRCLE / ACC_RESOLUTION;
		}
		if (y < 0) {
			y = y * ACC_TILT_QUATER_CIRCLE / ACC_RESOLUTION * -1;
		} else {
			y = ACC_TILT_FULL_CIRCLE - (y * ACC_TILT_QUATER_CIRCLE / ACC_RESOLUTION);
		}
	} else {
		x = ACC_TILT_HALF_CIRCLE - (x * ACC_TILT_QUATER_CIRCLE / ACC_RESOLUTION);

		if (y < 0) {
			y = ACC_TILT_HALF_CIRCLE - (y* ACC_TILT_QUATER_CIRCLE / ACC_RESOLUTION * -1);
		} else {
			y = ACC_TILT_HALF_CIRCLE + (y * ACC_TILT_QUATER_CIRCLE / ACC_RESOLUTION);
		}
	}
	printf("x = %3d , y = %3d\r\n", x, y);
}

/**
 * @brief Acting as pedometer
 * @param z Z-axis
 */
void IMP_Pedometer(int16_t z) {
	/* Each step can be divided to three parts:
	 * ascending - we start detecting step when device is being brought up over Z-axis
	 * descending - each step is known to end with slight oscillation when foot hits the ground
	 * return to idle state - waiting for next step
	 */
	if (z > ACC_PEDOMETER_HIGH_THRESHOLD) {
		pedometer_status = ACC_RISING_UP;
	} else if (z < ACC_PEDOMETER_LOW_THRESHOLD) {
		if (pedometer_status == ACC_RISING_UP) {
			pedometer_status = ACC_RISING_DOWN;
		}
	} else {
		if (pedometer_status == ACC_RISING_DOWN) {
			pedometer_status = ACC_PEDOMETER_STILL;
			printf("Step detected\n");
		}
	}
}

/**
 * @brief Speed detector
 * @param x X-axis
 */
void IMP_Speed(int16_t x) {
	/* Detecting threshold in idle state */
	if (sample_counter < ACC_VELOCITY_CALIB_SAMPLES) {
		sample_x += x;
		sample_counter++;
	} else {
		/* Obtain sample level for normalization */
		sample_x = sample_x >> 4;
		sample_counter = 0;
		/* Normalize level */
		cur_accelaration = x - sample_x;
		/* If below certain threshold, keep as zero */
		if (cur_accelaration < ACC_VELOCITY_IDLE_THRESHOLD  && cur_accelaration > -ACC_VELOCITY_IDLE_THRESHOLD ) {
			cur_velocity = 0;
		} else {
			/* Integrate accelaration */
			cur_velocity = prev_velocity + prev_accelaration + ((cur_accelaration - prev_accelaration) >> 1);
			/* Normalisation */
			cur_velocity = cur_velocity * ACC_VELOCITY_FLOAT_NORM / ACC_RESOLUTION;
		}
		IMP_PrintFloat(cur_velocity);
		/* Prepare for new execution */
		prev_accelaration = cur_accelaration;
		sample_x = 0;
	}
}
