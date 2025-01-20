#include "Cabecera/I2C.h"
#include "Cabecera/ADC.h"
#include "Cabecera/Font5x7.h"
#include "Cabecera/OLED_SSD1306_I2C.h"
#include "Cabecera/RTC_DS1307.h"
#include "Cabecera/GPIO.h"
#include "Cabecera/SHT30.h"
#include "Cabecera/UART.h"

#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"

/*****************************************
 * Defines
 * **************************************/

//#define ADC_BASE ADC0
//#define ADC_PIN_THERMISTOR1 0U // Canal del termistor 1


/*****************************************
 * Variables
 * **************************************/


char buffer2 [64];
char buffer3 [64];
char LecturaADC [64];
char mensaje [64];



/*****************************************
 * Code
 * **************************************/

int main(void){

	NVIC_SetPriority(USART1_IRQn, 1);		// Alta prioridad para UART
	NVIC_SetPriority(I2C1_IRQn, 3);   		// Baja prioridad para I2C
	//NVIC_SetPriority(ADC0_SEQA_IRQn, 3);	// Baja prioridad para ADC


	//Inicializaciones

	BOARD_InitDebugConsole();

	I2C_Init();


	//setCurrentTime();

	InitADC((1 << 0) | (1 << 1)); // Habilita los canales 0 y 1

	OLED_Init();
	OLED_Clear();
	OLED_Refresh();

	Init_UART();

	SHT30_SoftReset(I2C1_BASE);


	//Variables requeridas
	sht30_data_t data;
	//uint8_t hours, minutes, seconds;
	//uint8_t day, month, year;

	uint32_t adcResult0 = 0, adcResult1 = 0;
	float inyeccion = 0, retorno = 0, saltoTermico = 0;

	while (1) {
		// Disparar la conversión del ADC
		ADC_StartConversion(ADC0);

		// Leer el resultado del canal 0 (Inyección)
		if (ADC_GetChannelResult(ADC0, 0, &adcResult0)) {
			inyeccion = ConvertADCToTemperatureBeta(adcResult0); // Convierte a temperatura
		}

		// Leer el resultado del canal 1 (Retorno)
		if (ADC_GetChannelResult(ADC0, 1, &adcResult1)) {
			retorno = ConvertADCToTemperatureBeta(adcResult1); // Convierte a temperatura
		}

		// Calcular el salto térmico
		saltoTermico = retorno - inyeccion;

		// Leer datos del SHT30
		if (SHT30_ReadData(I2C1_BASE, &data) == kStatus_Success) {
			data.dewpoint = SHT30_CalculateDewPoint(data.temperature, data.humidity);
		}

		// Enviar datos por UART
		SendDataUART(inyeccion, retorno, saltoTermico, data);

		// Actualizar la pantalla OLED con los datos obtenidos
		UpdateOLED(inyeccion, retorno, saltoTermico, data);

		// Pausa de 500 ms para dar sensación de tiempo real
		SDK_DelayAtLeastUs(500000, SystemCoreClock); // 500 ms
	}
		/*
		// Disparar la conversión del ADC
		ADC_StartConversion(ADC0);


		// Leer el resultado de la conversión
		if (ADC_GetChannelResult(ADC0, 0, &adcResult0)) { // Canal 0
			sprintf(LecturaADC,"Resultado del ADC0: %lu\r\n", adcResult0);
		} else {
			sprintf(LecturaADC,"Esperando resultado del ADC0...\r\n");
		}
		UART_WriteString(USART1, LecturaADC);
		temperatura = ConvertADCToTemperatureBeta(adcResult0);
		sprintf(mensaje,"Temperatura en el ADC0: %lu\r\n", (int)temperatura);
		UART_WriteString(USART1, mensaje);

		// Leo el otro ADC

		// Leer el resultado de la conversión
		if (ADC_GetChannelResult(ADC0, 1, &adcResult1)) { // Canal 1
			sprintf(LecturaADC,"Resultado del ADC1: %lu\r\n", adcResult1);
		} else {
			sprintf(LecturaADC,"Esperando resultado del ADC1...\r\n");
		}
		UART_WriteString(USART1, LecturaADC);
		temperatura = ConvertADCToTemperatureBeta(adcResult1);
		sprintf(mensaje,"Temperatura en el ADC1: %lu\r\n", (int)temperatura);
		UART_WriteString(USART1, mensaje);


	    // Leer datos del SHT30
		if (SHT30_ReadData(I2C1_BASE, &data) == kStatus_Success) {
			data.dewpoint = SHT30_CalculateDewPoint(data.temperature, data.humidity);
			sprintf(buffer2, "Temp: %d C Hum: %d%% DewPt: %d C\r\n",
					(int)data.temperature, (int)data.humidity, (int)data.dewpoint);
			UART_WriteString(USART1, buffer2);
		}
		*/
	    /*
	    // Leer hora y fecha del RTC
	    if (DS1307_GetTime(&hours, &minutes, &seconds) == kStatus_Success &&
	        DS1307_GetDate(&day, &month, &year) == kStatus_Success) {
	        sprintf(buffer3, "Date: %02d/%02d/%02d Time: %02d:%02d:%02d\r\n",
	                day, month, year, hours, minutes, seconds);
	        UART_WriteString(USART1, buffer3);
	    }
		*/

	    // Pausa de 1 segundo
	    //SDK_DelayAtLeastUs(1000000, SystemCoreClock);

}

