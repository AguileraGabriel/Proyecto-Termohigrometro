/*
 * I2C.c
 *
 *  Created on: 14 ene. 2025
 *      Author: aguil
 */

#include "../Cabecera/I2C.h"

int baudRate = 50000, frequency = 1000000;

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

void I2C_ResetBus(I2C_Type *base) {
    if (I2C_GetStatusFlags(base) & 0x3U) {
        I2C_MasterReset(base); // Forzar un STOP en el bus
    }
}
