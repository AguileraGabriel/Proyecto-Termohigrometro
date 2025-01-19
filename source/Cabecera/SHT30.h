/*
 * SHT30.h
 *
 *  Created on: 10 ene. 2025
 *      Author: aguil
 */

#ifndef CABECERA_SHT30_H_
#define CABECERA_SHT30_H_



#include "fsl_i2c.h"  // Biblioteca del MCUXpresso SDK para manejar I2C
#include <math.h>  // Para log y operaciones matemáticas


// Dirección I2C del SHT30 (ajustar según configuración del sensor)
#define SHT30_I2C_ADDRESS 0x45

// Comandos del SHT30
#define SHT30_CMD_MEASURE_HIGHREP 0x2400  // Alta repetibilidad
#define SHT30_CMD_MEASURE_MEDREP  0x240B  // Media repetibilidad
#define SHT30_CMD_MEASURE_LOWREP  0x2416  // Baja repetibilidad
#define SHT30_CMD_SOFT_RESET      0x30A2  // Reinicio del sensor

// Estructura para los datos de temperatura y humedad
typedef struct {
    float temperature;
    float humidity;
    float dewpoint;
} sht30_data_t;

// Función para inicializar el sensor
status_t SHT30_Init(I2C_Type *base, uint32_t srcClock_Hz);

// Función para realizar mediciones
status_t SHT30_ReadData(I2C_Type *base, sht30_data_t *data);

// Función para reiniciar el sensor
status_t SHT30_SoftReset(I2C_Type *base);

//Funcion para calculor el punto de rocio
float SHT30_CalculateDewPoint(float temperature, float humidity);

#endif /* CABECERA_SHT30_H_ */
