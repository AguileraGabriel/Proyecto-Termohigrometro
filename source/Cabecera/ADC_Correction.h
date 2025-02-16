/*
 * ADC_Correction.h
 *
 *  Created on: 16 feb. 2025
 *      Author: aguil
 */

#ifndef CABECERA_ADC_CORRECTION_H_
#define CABECERA_ADC_CORRECTION_H_


/*
 * Calcula la tabla de diferencias divididas y almacena en el arreglo 'a'
 * los coeficientes del polinomio en forma de Newton:
 * a[0] = f[x0], a[1] = f[x0, x1], …, a[N-1] = f[x0, x1, …, x_{N-1}]
 */
void ADC_Correction_Init(void);

/*
 * Evalúa el polinomio de Newton en el punto X.
 * P(x) = a[0] + a[1]*(x - x[0]) + a[2]*(x - x[0])*(x - x[1]) + ... +
 *        a[N-1]*(x - x[0])*(x - x[1])*...*(x - x[N-2])
 */
double ADC_Correction_Apply(double X);



#endif /* CABECERA_ADC_CORRECTION_H_ */
