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


char buffer3 [64];



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
	//SHT30
	sht30_data_t data;

	//ADC
	uint32_t adcResult0 = 0, adcResult1 = 0;
	float inyeccion = 0, retorno = 0, saltoTermico = 0;

	//OLED
	const tImage Copodenieve = { image_data_Copodenieve, 24, 24, 8 };
	const tImage Sol = { image_data_Sol, 24, 24, 8 };
	const tImage Termometro = { image_data_Termometro, 24, 24, 8 };



	ShowIconAndText(Copodenieve,"Modo Refrigeracion");

	ShowIconAndText(Sol,"Modo Calefaccion");

	ShowIconAndText(Termometro,"Modo Termohigrometro");


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

		// Obtener fecha y hora del RTC
		rtc_datetime_t datetime = GetRTCDateTime();

		// Enviar datos por UART en formato JSON
		SendDataUART_JSON(inyeccion, retorno, saltoTermico, data, datetime);

		// Enviar datos por UART en formato JSON
		//SendDataUART_JSON(inyeccion, retorno, saltoTermico, data);


		// Actualizar la pantalla OLED con los datos obtenidos
		UpdateOLED(inyeccion, retorno, saltoTermico, data);


		// Pausa de 250 ms para dar sensación de tiempo real
		SDK_DelayAtLeastUs(250000, SystemCoreClock); // 250 ms
	}
}

