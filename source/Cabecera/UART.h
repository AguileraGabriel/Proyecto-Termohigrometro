/*
 * UART.h
 *
 *  Created on: 10 ene. 2025
 *      Author: aguil
 */

#ifndef CABECERA_UART_H_
#define CABECERA_UART_H_


#include "board.h"
#include "fsl_usart.h"
#include "fsl_swm.h"
#include "pin_mux.h"
#include <stdbool.h>
#include <stdio.h>
#include "SHT30.h"
#include "RTC_DS1307.h"

#include "Iconos.h"


//---------------------------------------------------------------//

//---------------------------------------------------------------//
// Defines
//---------------------------------------------------------------//




//---------------------------------------------------------------//
// Prototipos
//---------------------------------------------------------------//

/*
 * Inicializo UART en los pines 25 y 24, los cuales pueden ser modificados
 * La configuracion del UART es por defecto
 */
void Init_UART(void);

/*Transmito por USART
 *
 *  *base: Base de la USART que uso, en este caso USART1
 *  data: informacion a enviar
 *  texto: indico formato y texto que quiero enviar. Debe estar como "Temp: %d"
 *
 */

void Send_USART(USART_Type *base, uint8_t data, char texto);


void UART_WriteString(USART_Type *base, const char *data);

/***************************
 * Propias del Proyecto
 * *************************/

void SendDataUART(float inyeccion, float retorno, float saltoTermico, sht30_data_t data);

//void SendDataUART_JSON(float inyeccion, float retorno, float saltoTermico, sht30_data_t data);


void SendDataUART_JSON(Modo modo, float inyeccion, float retorno, float saltoTermico, sht30_data_t data, rtc_datetime_t datetime);



/*
 * Si recibo informacion por USART desde la computadora, la leo y
 * almaceno en dataUsart.
 */
void USART1_IRQHandler(void);


#endif /* CABECERA_UART_H_ */
