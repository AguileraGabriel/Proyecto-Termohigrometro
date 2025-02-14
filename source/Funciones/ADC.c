/*
 *
 *
 *
 */

#include "../Cabecera/ADC.h"

// Conjunto de 13 puntos (los nodos x y los valores f(x))
double x[N] = {
    1898.591, 1980.0, 2059.909, 2196.409, 2394.955,
    2556.273, 2655.545, 2779.636, 2965.773, 3139.500,
    3251.182, 3338.045, 3412.500
};

double y[N] = {
    1355.661, 1450.0, 1544.700, 1815.428, 2072.718,
    2276.686, 2467.859, 2627.246, 2836.527, 2982.520,
    3121.189, 3218.496, 3317.580
};

double a[N];



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
	computeDividedDifferences(x, y, a, N);
    float adcValueCorregido = newtonPolynomial((double)adcValue, x, a, N);;
	float voltage = ((float)adcValueCorregido * V_REF / 4095.0); // Conversión ADC a voltaje (12 bits)
    float rThermistor = R_REF * ((V_REF/(voltage) - 1)); // Resistencia del termistor

    // Evitar divisiones por cero o valores inválidos
    if (rThermistor <= 0) {
        return -273.15; // Error: temperatura imposible (por debajo del cero absoluto)
    }

    // Ecuación de Steinhart-Hart
    float logR = log(rThermistor); // ln(R)
    float terminoB = B * logR;
    float terminoC = C * logR * logR * logR;
    float tempKelvin = 1.0 / (A + terminoB + terminoC); // Kelvin

    //float tempKelvin = 1.0 / (A + B * logR + C * logR * logR * logR); // Kelvin
    float tempCelsius = tempKelvin - 273.15; // Convertir a Celsius


    return tempCelsius;
}

// Convierte el valor del ADC a temperatura en Celsius usando la ecuación β
float ConvertADCToTemperatureBeta(uint32_t adcValue) {
    float voltage = ((float)adcValue * V_REF / 4095.0); // Conversión ADC a voltaje (12 bits)
    float rThermistor = R_REF * ((V_REF/(voltage ))- 1); // Resistencia del termistor

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
 * Calcula la tabla de diferencias divididas y almacena en el arreglo 'a'
 * los coeficientes del polinomio en forma de Newton:
 * a[0] = f[x0], a[1] = f[x0, x1], …, a[N-1] = f[x0, x1, …, x_{N-1}]
 */
void computeDividedDifferences(double x[], double y[], double a[], int n) {
    double table[N][N];
    // Inicializamos la primera columna con los valores de y
    for (int i = 0; i < n; i++) {
        table[i][0] = y[i];
    }
    // Calculamos las diferencias divididas de orden j
    for (int j = 1; j < n; j++) {
        for (int i = 0; i < n - j; i++) {
            table[i][j] = (table[i+1][j-1] - table[i][j-1]) / (x[i+j] - x[i]);
        }
    }
    // Los coeficientes son la primera fila de la tabla
    for (int i = 0; i < n; i++) {
        a[i] = table[0][i];
    }
}

/*
 * Evalúa el polinomio de Newton en el punto X.
 * P(x) = a[0] + a[1]*(x - x[0]) + a[2]*(x - x[0])*(x - x[1]) + ... +
 *        a[N-1]*(x - x[0])*(x - x[1])*...*(x - x[N-2])
 */
double newtonPolynomial(double X, double x[], double a[], int n) {
    double result = a[0];
    double term = 1.0;
    for (int i = 1; i < n; i++) {
        term *= (X - x[i-1]);
        result += a[i] * term;
    }
    return result;
}

