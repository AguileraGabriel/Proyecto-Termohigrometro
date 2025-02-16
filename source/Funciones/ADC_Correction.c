/*
 * ADC_Correction.c
 *
 *  Created on: 16 feb. 2025
 *      Author: aguil
 */

#include "../Cabecera/ADC_Correction.h"

#define N 15

// Conjunto de 13 puntos (nodos x y valores f(x))
double x[N] = {
    1550.000,
	1600.000,
	1749.295,
	1898.591,
	2059.909,
	2196.409,
	2394.955,
    2556.273,
	2655.545,
	2779.636,
	2965.773,
	3139.500,
    3251.182,
	3338.045,
	3412.500
};

double y[N] = {
    500.000,
	952.880,
	1166.999,
	1355.661,
	1544.700,
	1815.428,
	2072.718,
    2276.686,
	2467.859,
	2627.246,
	2836.527,
	2982.520,
    3121.189,
	3218.496,
	3317.580
};

double a[N];

/*
 * Esta función calcula la tabla de diferencias divididas y almacena
 * en el arreglo 'a' los coeficientes del polinomio en forma de Newton:
 * a[0] = f[x0], a[1] = f[x0, x1], ..., a[N-1] = f[x0, ..., x_{N-1}]
 */
void ADC_Correction_Init() {
    double table[N][N];
    // Inicializar la primera columna con los valores de y
    for (int i = 0; i < N; i++) {
        table[i][0] = y[i];
    }
    // Calcular las diferencias divididas de orden j
    for (int j = 1; j < N; j++) {
        for (int i = 0; i < N - j; i++) {
            table[i][j] = (table[i+1][j-1] - table[i][j-1]) / (x[i+j] - x[i]);
        }
    }
    // Los coeficientes son la primera fila de la tabla
    for (int i = 0; i < N; i++) {
        a[i] = table[0][i];
    }
}

/*
 * Evalúa el polinomio de Newton en el punto X usando la forma acumulativa:
 * P(x) = a[0] + a[1]*(x - x[0]) + a[2]*(x - x[0])*(x - x[1]) + ... +
 *        a[N-1]*(x - x[0])*(x - x[1])*...*(x - x[N-2])
 */
double ADC_Correction_Apply(double X) {
    double result = a[0];
    double term = 1.0;
    for (int i = 1; i < N; i++) {
        term *= (X - x[i-1]);
        result += a[i] * term;
    }
    return result;
}
