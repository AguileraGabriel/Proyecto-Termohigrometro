/*
 * I2C.c
 *
 *  Created on: 14 ene. 2025
 *      Author: aguil
 */

#include "../Cabecera/I2C.h"

int baudRate = 4000, frequency = 50000;

/*--------------------------------------
 * I2C Initialization Function
 *--------------------------------------*/
void I2C_Init(void) {
    BOARD_InitDebugConsole();

    CLOCK_Select(kI2C1_Clk_From_MainClk);
    CLOCK_EnableClock(kCLOCK_Swm);
    SWM_SetMovablePinSelect(SWM0, kSWM_I2C1_SDA, kSWM_PortPin_P0_27); // Pin 13
    SWM_SetMovablePinSelect(SWM0, kSWM_I2C1_SCL, kSWM_PortPin_P0_26); // Pin 12
    CLOCK_DisableClock(kCLOCK_Swm);

    i2c_master_config_t masterConfig;
    I2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = baudRate;
    I2C_MasterInit(I2C1_BASE, &masterConfig, frequency);
}

