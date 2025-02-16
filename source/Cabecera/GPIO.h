/*
 * GPIO.h
 *
 *  Created on: 10 ene. 2025
 *      Author: aguil
 */

#ifndef CABECERA_GPIO_H_
#define CABECERA_GPIO_H_

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LPC845.h"
#include "fsl_debug_console.h"
#include "stdbool.h"

#define Switch_1 10 //Pulsador en PIN 23
#define Switch_2 11 //Pulsador en PIN 22
#define Switch_3 30 //Pulsador en PIN 16


#define G_LED 29 //LED Rojo Board
#define R_LED 8 //LED Verde Board
#define Y_LED 9 //LED Azul Board

#define LED_ON 0
#define LED_OFF 1

#define DEBOUNCE_DELAY_US 50000 // 50 ms (ajustable según las características del botón)

void Init_PinIn(void);

void Init_PinOut(void);


void prendeLEDRef(float saltoTermico);
void prendeLEDCal(float saltoTermico);

bool leerApretarBotonAceptacion(void);
bool leerApretarBotonCambio(void);




#endif /* CABECERA_GPIO_H_ */
