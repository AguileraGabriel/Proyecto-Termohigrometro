/*
 * ADC.h
 *
 *  Created on: 10 ene. 2025
 *      Author: aguil
 */

#ifndef CABECERA_ADC_H_
#define CABECERA_ADC_H_


#include "fsl_adc.h"
#include "fsl_iocon.h"

// Estructura para la configuración del ADC
typedef struct {
    ADC_Type *base;             // Base del periférico ADC
    uint32_t channelMask;       // Máscara de canales que formarán parte de ConvSeqA
    uint32_t triggerSource;     // Fuente de disparo (por software o hardware)
} my_adc_config_t;

// Inicialización del ADC con ConvSeqA
void ADCInit(my_adc_config_t *config);

// Inicia una conversión mediante el trigger configurado
void ADC_StartConversion(ADC_Type *base);

// Obtiene el resultado de un canal específico (no bloqueante)
bool ADC_GetChannelResult(ADC_Type *base, uint32_t channel, uint32_t *result);


#endif /* CABECERA_ADC_H_ */
