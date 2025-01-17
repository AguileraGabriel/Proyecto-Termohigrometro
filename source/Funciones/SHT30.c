/*
 * SHT30.c
 *
 *  Created on: 14 ene. 2025
 *      Author: aguil
 */

#include "../Cabecera/SHT30.h"

// Función para escribir comandos al sensor
static status_t SHT30_WriteCommand(I2C_Type *base, uint16_t command) {
    uint8_t cmdBuffer[2];
    cmdBuffer[0] = (command >> 8) & 0xFF;  // Byte alto
    cmdBuffer[1] = command & 0xFF;         // Byte bajo

    return I2C_MasterStart(base, SHT30_I2C_ADDRESS, kI2C_Write) == kStatus_Success &&
           I2C_MasterWriteBlocking(base, cmdBuffer, 2, kI2C_TransferDefaultFlag) == kStatus_Success &&
           I2C_MasterStop(base) == kStatus_Success
           ? kStatus_Success
           : kStatus_Fail;
}

// Inicialización del sensor
status_t SHT30_Init(I2C_Type *base, uint32_t srcClock_Hz) {
    i2c_master_config_t masterConfig;

    I2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = 100000;  // Velocidad estándar I2C
    I2C_MasterInit(base, &masterConfig, srcClock_Hz);

    return SHT30_SoftReset(base);
}

// Realizar una medición
status_t SHT30_ReadData(I2C_Type *base, sht30_data_t *data) {
    if (SHT30_WriteCommand(base, SHT30_CMD_MEASURE_HIGHREP) != kStatus_Success) {
        return kStatus_Fail;
    }

    // Esperar la conversión (~15ms para alta repetibilidad)
    SDK_DelayAtLeastUs(15000, SystemCoreClock);

    uint8_t rxBuffer[6];
    if (I2C_MasterStart(base, SHT30_I2C_ADDRESS, kI2C_Read) != kStatus_Success ||
        I2C_MasterReadBlocking(base, rxBuffer, 6, kI2C_TransferDefaultFlag) != kStatus_Success ||
        I2C_MasterStop(base) != kStatus_Success) {
        return kStatus_Fail;
    }

    // Convertir los datos
    uint16_t rawTemp = (rxBuffer[0] << 8) | rxBuffer[1];
    uint16_t rawHum = (rxBuffer[3] << 8) | rxBuffer[4];

    data->temperature = -45.0f + 175.0f * ((float)rawTemp / 65535.0f);
    data->humidity = 100.0f * ((float)rawHum / 65535.0f);

    return kStatus_Success;
}

// Reiniciar el sensor
status_t SHT30_SoftReset(I2C_Type *base) {
    return SHT30_WriteCommand(base, SHT30_CMD_SOFT_RESET);
}

