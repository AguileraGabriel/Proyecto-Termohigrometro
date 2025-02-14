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
#include "fsl_power.h"
#include "fsl_swm_connections.h"
#include <math.h> // Para funciones log y pow


// Constantes para el termistor (ajusta según tu hoja de datos)
#define A 0.000144 // Constante A de Steinhart-Hart
#define B 0.0004119 // Constante B de Steinhart-Hart
#define C -0.000000746 // Constante C de Steinhart-Hart
#define R_REF 10000.0      // Resistencia de referencia (10kΩ)
#define V_REF 3.3         // Voltaje de referencia del divisor (3.3V)

// Constantes del termistor
//#define BETA 3950.0      // Valor típico del coeficiente beta (ajusta según el termistor)
#define BETA 4466.45      // Valor típico del coeficiente beta (ajusta según el termistor) 12750.0
#define R_25 10000.0      // Resistencia del termistor a 25°C (10kΩ)
#define T_25 298.15      // Temperatura en Kelvin a 25°C


// Constante para calculo de interpolacion
#define N 13


// Estructura para la configuración del ADC
typedef struct {
    ADC_Type *base;             // Base del periférico ADC
    uint32_t channelMask;       // Máscara de canales que formarán parte de ConvSeqA
    uint32_t triggerSource;     // Fuente de disparo (por software o hardware)
} my_adc_config_t;

// Inicialización del ADC con ConvSeqA
void InitADC(uint32_t channelMask);

// Inicia una conversión mediante el trigger configurado
void ADC_StartConversion(ADC_Type *base);

// Obtiene el resultado de un canal específico (no bloqueante)
bool ADC_GetChannelResult(ADC_Type *base, uint32_t channel, uint32_t *result);

//Convierto el valor del ADC en temperatura
float ConvertADCToTemperature(uint32_t adcValue);

float ConvertADCToTemperatureBeta(uint32_t adcValue);

/*
 * Calcula la tabla de diferencias divididas y almacena en el arreglo 'a'
 * los coeficientes del polinomio en forma de Newton:
 * a[0] = f[x0], a[1] = f[x0, x1], …, a[N-1] = f[x0, x1, …, x_{N-1}]
 */
void computeDividedDifferences(double x[], double y[], double a[], int n);

/*
 * Evalúa el polinomio de Newton en el punto X.
 * P(x) = a[0] + a[1]*(x - x[0]) + a[2]*(x - x[0])*(x - x[1]) + ... +
 *        a[N-1]*(x - x[0])*(x - x[1])*...*(x - x[N-2])
 */
double newtonPolynomial(double X, double x[], double a[], int n);


//void InitADC_MultiChannel(uint32_t channelMask);


#endif /* CABECERA_ADC_H_ */
