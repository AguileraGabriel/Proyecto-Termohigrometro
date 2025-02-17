/*
 * ADC_Correction.c
 *
 *  Created on: 16 feb. 2025
 *      Author: aguil
 */

#include "../Cabecera/ADC_Correction.h"


double ADC_Correction_Apply(double X) {

    return X * 1.3113575268282631 - 1103.5179825286384;
}
