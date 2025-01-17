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

#define Switch_1 28 //Pulsador en PIN 14
#define Switch_2 29 //Pulsador en PIN 15
#define Switch_3 30 //Pulsador en PIN 16


#define R_LED 2 //LED Rojo Board
#define G_LED 0 //LED Verde Board
#define B_LED 1 //LED Azul Board

#define LED_ON 0
#define LED_OFF 1



#endif /* CABECERA_GPIO_H_ */
