/*
 * pinout.c
 *
 *  Created on: 28 oct. 2024
 *      Author: aguil
 */
#include "../Cabecera/GPIO.h"


//Inicio los pines como entradas
void Init_PinIn(void){
	gpio_pin_config_t in_config = {kGPIO_DigitalInput};

	//Inicio puerto y pines del GPIO_0
	GPIO_PortInit(GPIO, 0);
	GPIO_PinInit(GPIO, 0, Switch_1, &in_config); //Arriba
	GPIO_PinInit(GPIO, 0, Switch_2, &in_config); //Abajo
	GPIO_PinInit(GPIO, 0, Switch_3, &in_config); //OK
	return;
}



// Inicialización de pines para salidas
void Init_PinOut(void){
	gpio_pin_config_t out_config = {kGPIO_DigitalOutput, 1};
	GPIO_PortInit(GPIO, 1);
	GPIO_PinInit(GPIO, 1, R_LED, &out_config); //LED ROJO
	GPIO_PinInit(GPIO, 1, G_LED, &out_config); //LED VERDE
	GPIO_PinInit(GPIO, 1, B_LED, &out_config); //LED AZUL
	return;
}
