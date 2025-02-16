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
	GPIO_PinInit(GPIO, 0, Switch_1, &in_config); //Desplazar y Menu
	GPIO_PinInit(GPIO, 0, Switch_2, &in_config); //OK

	return;
}



// Inicialización de pines para salidas
void Init_PinOut(void){
	gpio_pin_config_t out_config = {kGPIO_DigitalOutput, 1};
	GPIO_PortInit(GPIO, 0);
	GPIO_PinInit(GPIO, 0, R_LED, &out_config); //LED ROJO
	GPIO_PinInit(GPIO, 0, G_LED, &out_config); //LED VERDE
	GPIO_PinInit(GPIO, 0, Y_LED, &out_config); //LED AZUL
	return;
}

void prendeLEDRef(float saltoTermico){
	if(saltoTermico>=8){
		GPIO_PinWrite(GPIO, 0, G_LED, 0); //Led Verde ON
		GPIO_PinWrite(GPIO, 0, R_LED, 1); //Led Rojo OFF
		GPIO_PinWrite(GPIO, 0, Y_LED, 1); //Led Azul OFF
	}else if(saltoTermico<=4){
		GPIO_PinWrite(GPIO, 0, G_LED, 1); //Led Verde OFF
		GPIO_PinWrite(GPIO, 0, R_LED, 0); //Led Rojo ON
		GPIO_PinWrite(GPIO, 0, Y_LED, 1); //Led Azul OFF
	}else{
		GPIO_PinWrite(GPIO, 0, G_LED, 1); //Led Verde OFF
		GPIO_PinWrite(GPIO, 0, R_LED, 1); //Led Rojo OFF
		GPIO_PinWrite(GPIO, 0, Y_LED, 0); //Led Azul ON
	}
	return;
}

void prendeLEDCal(float saltoTermico){
	if(saltoTermico>=14){
		GPIO_PinWrite(GPIO, 0, G_LED, 0); //Led Verde ON
		GPIO_PinWrite(GPIO, 0, R_LED, 1); //Led Rojo OFF
		GPIO_PinWrite(GPIO, 0, Y_LED, 1); //Led Azul OFF
	}else if(saltoTermico<=7){
		GPIO_PinWrite(GPIO, 0, G_LED, 1); //Led Verde OFF
		GPIO_PinWrite(GPIO, 0, R_LED, 0); //Led Rojo ON
		GPIO_PinWrite(GPIO, 0, Y_LED, 1); //Led Azul OFF
	}else{
		GPIO_PinWrite(GPIO, 0, G_LED, 1); //Led Verde OFF
		GPIO_PinWrite(GPIO, 0, R_LED, 1); //Led Rojo OFF
		GPIO_PinWrite(GPIO, 0, Y_LED, 0); //Led Azul ON
	}
	return;
}




bool leerApretarBotonAceptacion(void) {
	// Verificar si Switch_1 está presionado y Switch_2 no lo está
	if (GPIO_PinRead(GPIO, 0, Switch_1) == 0 && GPIO_PinRead(GPIO, 0, Switch_2) == 1) {
		// Esperar un tiempo de debounce
		SDK_DelayAtLeastUs(DEBOUNCE_DELAY_US, SystemCoreClock);

		// Comprobar nuevamente si el estado sigue siendo el mismo
		if (GPIO_PinRead(GPIO, 0, Switch_1) == 0 && GPIO_PinRead(GPIO, 0, Switch_2) == 1) {
			// Esperar hasta que el botón sea liberado (liberación activa)
			while (GPIO_PinRead(GPIO, 0, Switch_1) == 0);
			return true; // Retornar verdadero solo si se detectó un pulsado estable
		}
	}

	return false; // Retornar falso si no se cumple la condición
}

bool leerApretarBotonCambio(void){
	// Verificar si Switch_2 está presionado y Switch_1 no lo está
	if (GPIO_PinRead(GPIO, 0, Switch_2) == 0 && GPIO_PinRead(GPIO, 0, Switch_1) == 1) {
		// Esperar un tiempo de debounce
		SDK_DelayAtLeastUs(DEBOUNCE_DELAY_US, SystemCoreClock);

		// Comprobar nuevamente si el estado sigue siendo el mismo
		if (GPIO_PinRead(GPIO, 0, Switch_2) == 0 && GPIO_PinRead(GPIO, 0, Switch_1) == 1) {
			// Esperar hasta que el botón sea liberado (liberación activa)
			while (GPIO_PinRead(GPIO, 0, Switch_2) == 0);
			return true; // Retornar verdadero solo si se detectó un pulsado estable
		}
	}

	return false; // Retornar falso si no se cumple la condición
}

/*
 * //Encender Rojo
	if(GPIO_PinRead(GPIO,0,23)==0) {
		GPIO_PinWrite(GPIO, 1, 2, 0);
	}
	else GPIO_PinWrite(GPIO, 1, 2, 1);
 *
 *
 */


/*
 * if(GPIO_PinRead(GPIO,0,00)==0 && GPIO_PinRead(GPIO,0,01)==1){
		delay_ms(TIEMPO_ANRE);
		while(GPIO_PinRead(GPIO,0,00)==0 && GPIO_PinRead(GPIO,0,01)==1){
			delay_ms(TIEMPO_ANRE);
			//Accion a realizar
			time=1;
		}
 *
 *
 *
 */
