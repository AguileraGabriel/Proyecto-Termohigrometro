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

adc_result_info_t adcResultInfoStruct;
int valor;


/*****************************************
 * Code
 * **************************************/

int main(void){

	NVIC_SetPriority(USART1_IRQn, 1);		// Alta prioridad para UART
	NVIC_SetPriority(I2C1_IRQn, 3);   		// Baja prioridad para I2C
	//NVIC_SetPriority(ADC0_SEQA_IRQn, 3);	// Baja prioridad para ADC


	//Inicializaciones

	//BOARD_InitDebugConsole();

	I2C_Init();

	//InitADC(0,kSWM_ADC_CHN0); //Inicializo canal 0 con PIO0_07 PIN 32
	//InitADC(1,kSWM_ADC_CHN1); //Inicializo canal 1 con PIO0_06 PIN 33
	//setCurrentTime();
	//InitADC_MultiChannel((1 << 7) | (1 << 6));
	InitADC((1 << 0) | (1 << 1)); // Habilita los canales 0 y 1

	Init_UART();


	//Variables requeridas

	sht30_data_t data;
	//uint8_t hours, minutes, seconds;
	//uint8_t day, month, year;
	uint32_t adcResult0 = 0;
	uint32_t adcResult1 = 0;
	uint32_t adcValueChannel7 = 0;
	uint32_t adcValueChannel6 = 0;
	uint32_t result;



	while (1) {
		uint32_t statusFlags = ADC_GetStatusFlags(ADC0);
		sprintf(mensaje, "Flags de estado del ADC: 0x%08lX\r\n", statusFlags);
		UART_WriteString(USART1, mensaje);


		// Disparar la conversión del ADC
		ADC_StartConversion(ADC0);
		statusFlags = ADC_GetStatusFlags(ADC0);
		sprintf(mensaje, "Flags de estado del ADC: 0x%08lX\r\n", statusFlags);
		UART_WriteString(USART1, mensaje);

		// Leer el resultado de la conversión
		if (ADC_GetChannelResult(ADC0, 0, &adcResult0)) { // Canal 0
			sprintf(LecturaADC,"Resultado del ADC0: %lu\r\n", adcResult0);
		} else {
			sprintf(LecturaADC,"Esperando resultado del ADC0...\r\n");
		}
		UART_WriteString(USART1, LecturaADC);

/*
		// Bucle para leer los resultados de la secuencia
		for (int i = 0; i < 8; i++) { // 8 es el número máximo de conversiones en la secuencia
			if (ADC_GetChannelConversionResult(ADC0, i, &result)) {
				if (i == 7) {
					adcValueChannel7 = result;
				} else if (i == 6) {
					adcValueChannel6 = result;
				}
			}
			//Si el valor de result no se actualiza, se ignora
		}
		sprintf(LecturaADC,"Canal 7: %d, Canal 6: %d\r\n", adcValueChannel7, adcValueChannel6);
		UART_WriteString(USART1, LecturaADC);
*/

		// Disparar la conversión del ADC
		//ADC_StartConversion(ADC0);
		statusFlags = ADC_GetStatusFlags(ADC0);
		sprintf(mensaje, "Flags de estado del ADC: 0x%08lX\r\n", statusFlags);
		UART_WriteString(USART1, mensaje);

		// Leer el resultado de la conversión
		if (ADC_GetChannelResult(ADC0, 1, &adcResult1)) { // Canal 1
			sprintf(LecturaADC,"Resultado del ADC1: %lu\r\n", adcResult1);
		} else {
			sprintf(LecturaADC,"Esperando resultado del ADC1...\r\n");
		}
		UART_WriteString(USART1, LecturaADC);



	    // Leer datos del SHT30
		if (SHT30_ReadData(I2C1_BASE, &data) == kStatus_Success) {
			data.dewpoint = SHT30_CalculateDewPoint(data.temperature, data.humidity);
			sprintf(buffer2, "Temp: %d C Hum: %d%% DewPt: %d C\r\n",
					(int)data.temperature, (int)data.humidity, (int)data.dewpoint);
			UART_WriteString(USART1, buffer2);
		}

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
	    SDK_DelayAtLeastUs(1000000, SystemCoreClock);
	}
}

