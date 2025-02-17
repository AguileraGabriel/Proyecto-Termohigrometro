/*
 * UART.c
 *
 *  Created on: 28 oct. 2024
 *      Author: aguil
 */

#include "../Cabecera/UART.h"

//---------------------------------------------------------------//
// Variables
//---------------------------------------------------------------//
uint8_t flagReceived = 0, dataUsart;

char buffer[64]; //Buffer donde cargo la informacion a enviar por USART.
char newline []="\r\n"; //Buffer para realizar un salto de linea.
char bufferUART[256];


/*
 * Inicializo UART en los pines 25 y 24, los cuales pueden ser modificados
 * La configuracion del UART es por defecto
 */
void Init_UART(void){
	// Habilito clock de matriz de conmutacion
	CLOCK_EnableClock(kCLOCK_Swm);
	// USART1_TXD es una funcion movible y la asigno al pin P0_25, puedo cambiar de pin si es necesario.
	SWM_SetMovablePinSelect(SWM0, kSWM_USART1_TXD, kSWM_PortPin_P0_25);	    // USART1_TXD connect to P0_25
	// USART1_RXD es una funcion movible y la asigno al pin P0_24, puedo cambiar de pin si es necesario.
	SWM_SetMovablePinSelect(SWM0, kSWM_USART1_RXD, kSWM_PortPin_P0_24);		// USART1_RXD connect to P0_24
	// Deshabilito clock de matriz de conmutacion
	CLOCK_DisableClock(kCLOCK_Swm);
	CLOCK_Select(kUART1_Clk_From_MainClk);
	/* Default config by using USART_GetDefaultConfig():
	 * config->baudRate_Bps = 9600U;
	 * config->parityMode = kUSART_ParityDisabled;
	 * config->stopBitCount = kUSART_OneStopBit;
	 * config->bitCountPerChar = kUSART_8BitsPerChar;
	 * config->loopback = false;
	 * config->enableRx = false;
	 * config->enableTx = false;
	 * config->syncMode = kUSART_SyncModeDisabled;
	 */
	usart_config_t config;
	USART_GetDefaultConfig(&config);
	/*En caso de querer cambiar el baudrate:
	 * config.baudRate_Bps = VALOR DECEADO
	 */

	//Habilito pines de Tx y Rx
	config.enableRx     = true;
	config.enableTx     = true;

	 /* Initialize the USART with configuration. */
	USART_Init(USART1, &config, CLOCK_GetFreq(kCLOCK_MainClk));
	//Habilito la interrupcion para USART
	USART_EnableInterrupts(USART1, kUSART_RxReadyInterruptEnable);
	NVIC_EnableIRQ(USART1_IRQn);

	//Escribo para verificar la conexión
	//USART_WriteByte(USART1, 0x31);
	return;
}




/*Transmito por USART
 *
 *  *base: Base de la USART que uso, en este caso USART1
 *  data: informacion a enviar
 *  texto: indico formato y texto que quiero enviar. Debe estar como "Temp: %d"
 *
 */

void Send_USART(USART_Type *base, uint8_t data, char texto){
	//Cargo los datos al buffer
	sprintf(buffer, texto,data);
	USART_WriteBlocking(base,&buffer,strlen(buffer));

	flagReceived = 0;
	return;
}


void UART_WriteString(USART_Type *base, const char *data) {
    while (*data) {
        while (!(base->STAT & USART_STAT_TXRDY_MASK)) {
            // Esperar a que el buffer esté listo
        }
        USART_WriteByte(base, *data++);
    }
}

/***************************
 * Propias del Proyecto
 * *************************/

//Envio por UART todos mis valores
void SendDataUART(float inyeccion, float retorno, float saltoTermico, sht30_data_t data) {
    // Enviar temperatura de Inyección (Termistor 1)
    sprintf(bufferUART, "Inyeccion: %0.2f C\r\n", inyeccion);
    UART_WriteString(USART1, bufferUART);

    // Enviar temperatura de Retorno (Termistor 2)
    sprintf(bufferUART, "Retorno: %0.2f C\r\n", retorno);
    UART_WriteString(USART1, bufferUART);

    // Enviar Salto Térmico
    sprintf(bufferUART, "Salto Termico: %0.2f C\r\n", saltoTermico);
    UART_WriteString(USART1, bufferUART);

    // Enviar datos del SHT30 - Temp Ref
    sprintf(bufferUART, "Temp Ref: %0.2f C\r\n", data.temperature);
    UART_WriteString(USART1, bufferUART);

    // Enviar datos del SHT30 - Hum Ref
    sprintf(bufferUART, "Hum Ref: %d%%\r\n", (int)data.humidity);
    UART_WriteString(USART1, bufferUART);

    // Enviar datos del SHT30 - Dew Point
    sprintf(bufferUART, "Dew Point: %0.2f C\r\n", data.dewpoint);
    UART_WriteString(USART1, bufferUART);

    // Línea en blanco para separar mediciones
    UART_WriteString(USART1, "\r\n");
}

/*
//Envio mis datos en formato JSON
void SendDataUART_JSON(float inyeccion, float retorno, float saltoTermico, sht30_data_t data) {
    // Generar mensaje JSON directamente con los valores flotantes
    sprintf(bufferUART,
            "{ \"inyeccion\": %.2f, \"retorno\": %.2f, \"saltoTermico\": %.2f, \"tempRef\": %.2f, \"humRef\": %d, \"dewPoint\": %.2f }\r\n",
            inyeccion, retorno, saltoTermico, data.temperature, (int)data.humidity, data.dewpoint);

    // Enviar el mensaje por UART
    UART_WriteString(USART1, bufferUART);
}
*/


void SendDataUART_JSON(Modo modo, float inyeccion, float retorno, float saltoTermico, sht30_data_t data, rtc_datetime_t datetime) {
    // Generar mensaje JSON directamente con los valores flotantes y fecha/hora
    sprintf(bufferUART,
            "{ \"modo\": %i, \"inyeccion\": %.2f, \"retorno\": %.2f, \"saltoTermico\": %.2f, \"tempRef\": %.2f, \"humRef\": %.2f, \"dewPoint\": %.2f, "
            "\"date\": \"%02d/%02d/%02d\", \"time\": \"%02d:%02d:%02d\" }\r\n",
            modo, inyeccion, retorno, saltoTermico, data.temperature, data.humidity, data.dewpoint,
            datetime.day, datetime.month, datetime.year, datetime.hours, datetime.minutes, datetime.seconds);

    // Enviar el mensaje por UART
    UART_WriteString(USART1, bufferUART);
}



//---------------------------------------------------------------//
//Interrupción
//---------------------------------------------------------------//
/*
 * Si recibo informacion por USART desde la computadora, la leo y
 * almaceno en dataUsart.
 */
//---------------------------------------------------------------//
void USART1_IRQHandler(void){
	dataUsart = USART_ReadByte(USART1);
	flagReceived = 1;
	return;
}
//---------------------------------------------------------------//
