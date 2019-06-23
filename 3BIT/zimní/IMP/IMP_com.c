/**
 * IMP Project 2018
 * Communication
 * @brief Communication
 * @author Matej Mitas, xmitas02
 * @file imp_com.c
 */

#include "IMP_com.h"
/* Variables - Communication*/
static i2c_master_handle_t device_handle;
static bool completionFlag = false;
static bool nakFlag = false;

/**
 * @brief MIGRATED EXAMPLE FUNCTION
 */
void i2c_release_bus_delay(void) {
    uint32_t i = 0;
    for (i = 0; i < I2C_RELEASE_BUS_COUNT; i++)
    {
        __NOP();
    }
}

/**
 * @brief MIGRATED EXAMPLE FUNCTION
 */
void BOARD_I2C_ConfigurePins(void) {
    /* Port D Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortD);

    const port_pin_config_t portd6_pin63_config = {/* Internal pull-up resistor is enabled */
                                                   kPORT_PullUp,
                                                   /* Fast slew rate is configured */
                                                   kPORT_FastSlewRate,
                                                   /* Passive filter is disabled */
                                                   kPORT_PassiveFilterDisable,
                                                   /* Low drive strength is configured */
                                                   kPORT_LowDriveStrength,
                                                   /* Pin is configured as I2C1_SDA */
                                                   kPORT_MuxAlt4};
    /* PORTD6 (pin 63) is configured as I2C1_SDA */
    PORT_SetPinConfig(PORTD, 6U, &portd6_pin63_config);

    const port_pin_config_t portd7_pin64_config = {/* Internal pull-up resistor is enabled */
                                                   kPORT_PullUp,
                                                   /* Fast slew rate is configured */
                                                   kPORT_FastSlewRate,
                                                   /* Passive filter is disabled */
                                                   kPORT_PassiveFilterDisable,
                                                   /* Low drive strength is configured */
                                                   kPORT_LowDriveStrength,
                                                   /* Pin is configured as I2C1_SCL */
                                                   kPORT_MuxAlt4};
    /* PORTD7 (pin 64) is configured as I2C1_SCL */
    PORT_SetPinConfig(PORTD, 7U, &portd7_pin64_config);
}

/**
 * @brief MIGRATED EXAMPLE FUNCTION
 */
void BOARD_I2C_ReleaseBus(void) {
    uint8_t i = 0;
    gpio_pin_config_t pin_config;
    port_pin_config_t i2c_pin_config = {0};

    /* Config pin mux as gpio */
    i2c_pin_config.pullSelect = kPORT_PullUp;
    i2c_pin_config.mux = kPORT_MuxAsGpio;

    pin_config.pinDirection = kGPIO_DigitalOutput;
    pin_config.outputLogic = 1U;
    CLOCK_EnableClock(kCLOCK_PortD);
    PORT_SetPinConfig(I2C_RELEASE_SCL_PORT, I2C_RELEASE_SCL_PIN, &i2c_pin_config);
    PORT_SetPinConfig(I2C_RELEASE_SCL_PORT, I2C_RELEASE_SDA_PIN, &i2c_pin_config);

    GPIO_PinInit(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, &pin_config);
    GPIO_PinInit(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, &pin_config);

    /* Drive SDA low first to simulate a start */
    GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
    i2c_release_bus_delay();

    /* Send 9 pulses on SCL and keep SDA high */
    for (i = 0; i < 9; i++) {
        GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
        i2c_release_bus_delay();

        GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
        i2c_release_bus_delay();

        GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
        i2c_release_bus_delay();
        i2c_release_bus_delay();
    }
    /* Send stop */
    GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
    i2c_release_bus_delay();

    GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
    i2c_release_bus_delay();
}

/**
 * @brief MIGRATED EXAMPLE FUNCTION
 */
void i2c_master_callback(I2C_Type *base, i2c_master_handle_t *handle, status_t status, void *userData) {
    /* Signal transfer success when received success status. */
    if (status == kStatus_Success) {
        completionFlag = true;
    }
    /* Signal transfer success when received success status. */
    if ((status == kStatus_I2C_Nak) || (status == kStatus_I2C_Addr_Nak)) {
        nakFlag = true;
    }
}

/**
 * @brief Common function for both read/write to registers
 * @param reg_addr address of the register to be read from
 * @param isWrite determine operation type
 * @param value register value to written
 * @param buffer buffer for read data
 * @param buffer_size size of the register
 * @return operation successful
 */
bool IMP_ComCarryOut(uint8_t reg_addr, bool isWrite, uint8_t value, uint8_t *buffer, uint32_t buffer_size) {
    /* Init wrapper */
    i2c_master_transfer_t transfer_opt;
    memset(&transfer_opt, 0, sizeof(transfer_opt));
    /* Different properties */
    if (isWrite) {
        transfer_opt.direction = kI2C_Write;
        transfer_opt.data = &value;
        transfer_opt.dataSize = 1;
    } else {
        transfer_opt.direction = kI2C_Read;
        transfer_opt.data = buffer;
        transfer_opt.dataSize = buffer_size;
    }
    /* Common properties */
    transfer_opt.slaveAddress = I2C_ADDR;
    transfer_opt.subaddress = reg_addr;
    transfer_opt.subaddressSize = 1;
    transfer_opt.flags = kI2C_TransferDefaultFlag;
    /* Utilizing library function */
    I2C_MasterTransferNonBlocking(BOARD_ACCEL_I2C_BASEADDR, &device_handle, &transfer_opt);
    /* Transfer */
    while ((!nakFlag) && (!completionFlag)){
    }
    nakFlag = false;
    if (completionFlag == true) {
        completionFlag = false;
        return true;
    }
    else {
        return false;
    }
}

/**
 * @brief Wrapper for reg write
 * @param reg_addr address of the register to be written to
 * @param value register value to written
 * @return operation successful
 */
bool IMP_ComSend(uint8_t reg_addr, uint8_t value) {
    return IMP_ComCarryOut(reg_addr, true, value, NULL, 0);
}

/**
 * @brief Wrapper for reg read
 * @param reg_addr address of the register to be read from
 * @param buffer buffer for read data
 * @param buffer_size size of the register
 * @return operation successful
 */
bool IMP_ComReceive(uint8_t reg_addr, uint8_t *buffer, uint32_t buffer_size) {
    return IMP_ComCarryOut(reg_addr, false, 0, buffer, buffer_size);
}

/**
 * @brief Merges splited accelerometer data
 * @param buffer containing our data
 * @param upper_half top 8 bits
 * @param lower_half bottom 8 bits
 * @return prepared number
 */
int16_t IMP_ComDataNorm(uint8_t *buffer, uint8_t upper_half, uint8_t lower_half) {
	return (int16_t)((buffer[upper_half] << 8) | buffer[lower_half]) >> 2;
}

/**
 * @brief I2C communication initiator and handler
 */
void IMP_ComInit(void) {
    /* Ensure we can't start our communication with device */
    BOARD_I2C_ReleaseBus();
    BOARD_I2C_ConfigurePins();
    /* Init device handle for usage in program */
    I2C_MasterTransferCreateHandle(BOARD_ACCEL_I2C_BASEADDR, &device_handle, i2c_master_callback, NULL);
    /* Fetch default config */
    i2c_master_config_t masterConfig;
    I2C_MasterGetDefaultConfig(&masterConfig);
    /* Set our parameters */
    masterConfig.baudRate_Bps = I2C_BAUDRATE;
    /* Set correct clock frequency */
    uint32_t sourceClock = ACCEL_I2C_CLK_FREQ;
    /* Device init with preconfigured parameters */
    I2C_MasterInit(BOARD_ACCEL_I2C_BASEADDR, &masterConfig, sourceClock);

    /* Set acceloremeter resolution to +-4g */
    IMP_ComSend(ACCEL_XYZ_DATA_CFG, 0x01);
    /* Config accelerometer with following parameters:
     * active, 16bit data, low noise, 200Hz sampling
     */
    IMP_ComSend(ACCEL_CTRL_REG1, 0x0d);
    /* Auxiliary variables for buffer and status  */
    uint8_t receive_buffer[7];
    uint8_t reader_status = 0;
    /* Receiving loop */
    for (uint32_t i = 0; i < ACCEL_READ_TIMES; i++) {
        reader_status = 0;
        /* New data ready? */
        while (reader_status != 0xff) {
        	IMP_ComReceive(ACCEL_STATUS, &reader_status, 1);
        }
        /* Receive new accelerometer data */
        IMP_ComReceive(ACCEL_STATUS, receive_buffer, 7);
        /* Submit data to program */
        IMP_BindAccData(
        	IMP_ComDataNorm(receive_buffer, 1, 2),
			IMP_ComDataNorm(receive_buffer, 3, 4),
			IMP_ComDataNorm(receive_buffer, 5, 6)
        );
    }
}
