/*
 *
 *
 *
 */

#include "../Cabecera/ADC.h"


// Dispara la conversión de ConvSeqA
void ADC_StartConversion(ADC_Type *base) {
    ADC_DoSoftwareTriggerConvSeqA(base);
}



// Obtiene el resultado del canal especificado (no bloqueante)

bool ADC_GetChannelResult(ADC_Type *base, uint32_t channel, uint32_t *result) {
    adc_result_info_t resultInfo;

    // Obtén el resultado directamente sin verificar el flag (solo para depuración)
    if (ADC_GetChannelConversionResult(base, channel, &resultInfo)) {
        *result = resultInfo.result; // Devuelve el valor convertido
        ADC_ClearStatusFlags(base, kADC_ConvSeqAInterruptFlag);
        return true;
    }
    return false; // No hay resultado disponible
}







// Inicialización del ADC
void InitADC(uint32_t channelMask) {
    // Configuración del switch matrix para los pines asociados al ADC
    CLOCK_EnableClock(kCLOCK_Swm);
    SWM_SetFixedPinSelect(SWM0, kSWM_ADC_CHN0, true); // Habilita el canal 0 (PIO0_7)
    SWM_SetFixedPinSelect(SWM0, kSWM_ADC_CHN1, true); // Habilita el canal 1 (PIO0_6)
    CLOCK_DisableClock(kCLOCK_Swm);

    // Configuración del reloj del ADC
    CLOCK_Select(kADC_Clk_From_Fro);
    CLOCK_SetClkDivider(kCLOCK_DivAdcClk, 1U);
    POWER_DisablePD(kPDRUNCFG_PD_ADC0);

    // Calibración del ADC
    uint32_t frequency = CLOCK_GetFreq(kCLOCK_Fro) / CLOCK_GetClkDivider(kCLOCK_DivAdcClk);
    (void)ADC_DoSelfCalibration(ADC0, frequency);

    // Configuración básica del ADC
    adc_config_t adcConfigStruct;
    adcConfigStruct.clockMode = kADC_ClockSynchronousMode; // Modo síncrono
    adcConfigStruct.clockDividerNumber = 1;                // Divisor del reloj
    adcConfigStruct.enableLowPowerMode = false;            // Sin modo de bajo consumo
    adcConfigStruct.voltageRange = kADC_HighVoltageRange;  // Rango de voltaje
    ADC_Init(ADC0, &adcConfigStruct);

    // Configuración de la secuencia de conversión
    adc_conv_seq_config_t adcConvSeqConfigStruct;
    adcConvSeqConfigStruct.channelMask = channelMask; // Máscara de canales (0 y 1)
    adcConvSeqConfigStruct.triggerMask = 0;           // Sin triggers externos
    adcConvSeqConfigStruct.triggerPolarity = kADC_TriggerPolarityPositiveEdge;
    adcConvSeqConfigStruct.enableSingleStep = false;  // Modo continuo
    adcConvSeqConfigStruct.enableSyncBypass = false;  // Sin bypass
    adcConvSeqConfigStruct.interruptMode = kADC_InterruptForEachSequence; // Interrupciones por secuencia
    ADC_SetConvSeqAConfig(ADC0, &adcConvSeqConfigStruct);
    ADC_EnableConvSeqA(ADC0, true); // Habilita la secuencia A
}

// Convierte el valor del ADC a temperatura en Celsius
float ConvertADCToTemperature(uint32_t adcValue) {
    float voltage = (float)adcValue * V_REF / 4095.0; // Conversión ADC a voltaje (12 bits)
    float rThermistor = R_REF * voltage / (V_REF - voltage); // Resistencia del termistor

    // Evitar divisiones por cero o valores inválidos
    if (rThermistor <= 0) {
        return -273.15; // Error: temperatura imposible (por debajo del cero absoluto)
    }

    // Ecuación de Steinhart-Hart
    float logR = log(rThermistor); // ln(R)
    float tempKelvin = 1.0 / (A + B * logR + C * logR * logR * logR); // Kelvin
    float tempCelsius = tempKelvin - 273.15; // Convertir a Celsius

    return tempCelsius;
}

// Convierte el valor del ADC a temperatura en Celsius usando la ecuación β
float ConvertADCToTemperatureBeta(uint32_t adcValue) {
    float voltage = (float)adcValue * V_REF / 4095.0; // Conversión ADC a voltaje (12 bits)
    float rThermistor = R_REF * voltage / (V_REF - voltage); // Resistencia del termistor

    // Evitar divisiones por cero o valores inválidos
    if (rThermistor <= 0) {
        return -273.15; // Error: temperatura imposible (por debajo del cero absoluto)
    }

    // Ecuación paramétrica de β
    float tempKelvin = BETA / (log(rThermistor / R_25) + (BETA / T_25)); // Temperatura en Kelvin
    float tempCelsius = tempKelvin - 273.15; // Convertir a Celsius

    return tempCelsius;
}


/*
void InitADC(uint32_t adcChannel, uint32_t pin)
{
    // Inicialización de la consola para depuración
    //BOARD_InitDebugConsole();

    // Configuración del switch matrix para el pin del ADC
    CLOCK_EnableClock(kCLOCK_Swm); // Habilitamos el reloj del SWM
    SWM_SetFixedPinSelect(SWM0, pin, true); // Asociamos el pin al ADC
    CLOCK_DisableClock(kCLOCK_Swm); // Deshabilitamos el reloj del SWM

    // Configuración del reloj del ADC
    CLOCK_Select(kADC_Clk_From_Fro); // Fuente de reloj: FRO
    CLOCK_SetClkDivider(kCLOCK_DivAdcClk, 1U); // Divisor del reloj ADC

    // Habilitamos el bloque de alimentación del ADC
    POWER_DisablePD(kPDRUNCFG_PD_ADC0);

    // Calibración del ADC
    uint32_t frequency = CLOCK_GetFreq(kCLOCK_Fro) / CLOCK_GetClkDivider(kCLOCK_DivAdcClk);
    (void)ADC_DoSelfCalibration(ADC0, frequency); // Calibramos el ADC con la frecuencia configurada

    // Configuración básica del ADC
    adc_config_t adcConfigStruct;
    adcConfigStruct.clockMode = kADC_ClockSynchronousMode; // Modo síncrono
    adcConfigStruct.clockDividerNumber = 1; // Divisor de reloj
    adcConfigStruct.enableLowPowerMode = false; // Deshabilitar modo de bajo consumo
    adcConfigStruct.voltageRange = kADC_HighVoltageRange; // Rango de voltaje
    ADC_Init(ADC0, &adcConfigStruct); // Inicializamos el ADC con los parámetros configurados

    // Configuración de la secuencia de conversión
    adc_conv_seq_config_t adcConvSeqConfigStruct;
    adcConvSeqConfigStruct.channelMask = (1 << adcChannel); // Máscara del canal pasado como argumento
    adcConvSeqConfigStruct.triggerMask = 0; // Sin triggers externos
    adcConvSeqConfigStruct.triggerPolarity = kADC_TriggerPolarityPositiveEdge; // Polaridad del trigger
    adcConvSeqConfigStruct.enableSingleStep = false; // Modo de pasos múltiples
    adcConvSeqConfigStruct.enableSyncBypass = false; // No bypass
    adcConvSeqConfigStruct.interruptMode = kADC_InterruptForEachSequence; // Interrupciones por secuencia
    ADC_SetConvSeqAConfig(ADC0, &adcConvSeqConfigStruct); // Aplicamos la configuración de la secuencia
    ADC_EnableConvSeqA(ADC0, true); // Habilitamos la secuencia configurada

    // Disparamos una conversión por software
    //ADC_DoSoftwareTriggerConvSeqA(ADC0);

    // Finalizamos la función
    return;
}*/

/*
// Función para inicializar el ADC para múltiples canales
void InitADC_MultiChannel(uint32_t channelMask)
{
    // Configuración del switch matrix para los pines del ADC.
    // Esta parte debe hacerse una sola vez para todos los pines que se van a usar.
    CLOCK_EnableClock(kCLOCK_Swm);

    // Ejemplo para PIO0_07 y PIO0_06 (ajusta según tus pines)
    if (channelMask & (1 << 7)) {
        SWM_SetFixedPinSelect(SWM0, kSWM_ADC_CHN7, true); // PIO0_07
    }
    if (channelMask & (1 << 6)) {
        SWM_SetFixedPinSelect(SWM0, kSWM_ADC_CHN6, true); // PIO0_06
    }

    CLOCK_DisableClock(kCLOCK_Swm);


    // Configuración del reloj del ADC (una sola vez)
    CLOCK_Select(kADC_Clk_From_Fro);
    CLOCK_SetClkDivider(kCLOCK_DivAdcClk, 1U);

    // Habilitamos el bloque de alimentación del ADC
    POWER_DisablePD(kPDRUNCFG_PD_ADC0);

    // Calibración del ADC (una sola vez)
    uint32_t frequency = CLOCK_GetFreq(kCLOCK_Fro) / CLOCK_GetClkDivider(kCLOCK_DivAdcClk);
    (void)ADC_DoSelfCalibration(ADC0, frequency);

    // Configuración básica del ADC (una sola vez)
    adc_config_t adcConfigStruct;
    adcConfigStruct.clockMode = kADC_ClockSynchronousMode;
    adcConfigStruct.clockDividerNumber = 1;
    adcConfigStruct.enableLowPowerMode = false;
    adcConfigStruct.voltageRange = kADC_HighVoltageRange;
    ADC_Init(ADC0, &adcConfigStruct);

    // Configuración de la secuencia de conversión para ambos canales
    adc_conv_seq_config_t adcConvSeqConfigStruct;
    adcConvSeqConfigStruct.channelMask = channelMask; // Máscara de canales (ej: (1 << 7) | (1 << 6))
    adcConvSeqConfigStruct.triggerMask = 0;
    adcConvSeqConfigStruct.triggerPolarity = kADC_TriggerPolarityPositiveEdge;
    adcConvSeqConfigStruct.enableSingleStep = false;
    adcConvSeqConfigStruct.enableSyncBypass = false;
    adcConvSeqConfigStruct.interruptMode = kADC_InterruptForEachSequence;
    ADC_SetConvSeqAConfig(ADC0, &adcConvSeqConfigStruct);
    ADC_EnableConvSeqA(ADC0, true);
}
*/
