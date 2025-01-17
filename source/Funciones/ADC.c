/*
 *
 *
 *
 */

#include "../Cabecera/ADC.h"

// Inicializa el ADC con ConvSeqA configurando los canales y el trigger
void ADCInit( my_adc_config_t *config) {
    adc_conv_seq_config_t convSeqConfig;

    // Configurar la secuencia ConvSeqA
    convSeqConfig.channelMask = config->channelMask; // Canales seleccionados
    convSeqConfig.triggerPolarity = kADC_TriggerPolarityPositiveEdge;
    //convSeqConfig.triggerSource = config->triggerSource; // Trigger configurado
    convSeqConfig.enableSingleStep = false;             // Secuencia completa
    convSeqConfig.enableSyncBypass = false;
    convSeqConfig.interruptMode = kADC_InterruptForEachSequence;


    // Configura ConvSeqA
    ADC_SetConvSeqAConfig(config->base, &convSeqConfig);
    ADC_EnableConvSeqA(config->base, true);

    // Habilita el ADC
    ADC_EnableHardwareTrigger(config->base, false); // Software trigger por defecto
    ADC_DoAutoCalibration(config->base);
}

// Dispara la conversión de ConvSeqA
void ADC_StartConversion(ADC_Type *base) {
    ADC_DoSoftwareTriggerConvSeqA(base);
}

// Obtiene el resultado del canal especificado (no bloqueante)
bool ADC_GetChannelResult(ADC_Type *base, uint32_t channel, uint32_t *result) {
    adc_result_info_t resultInfo;

    // Verifica si hay un resultado disponible para el canal
    if (ADC_GetChannelConversionResult(base, channel, &resultInfo)) {
        *result = resultInfo.result; // Devuelve el valor convertido
        return true;
    }
    return false; // No hay resultado disponible
}

