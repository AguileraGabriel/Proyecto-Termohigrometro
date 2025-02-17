#include "Cabecera/I2C.h"
#include "Cabecera/ADC.h"
#include "Cabecera/Font5x7.h"
#include "Cabecera/OLED_SSD1306_I2C.h"
#include "Cabecera/RTC_DS1307.h"
#include "Cabecera/GPIO.h"
#include "Cabecera/SHT30.h"
#include "Cabecera/UART.h"
#include "Cabecera/ADC_Correction.h"

#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "stdbool.h"

/*****************************************
 * Defines
 * **************************************/


/*****************************************
 * Variables
 * **************************************/

bool menuPrincipalActivo;

Modo modo;

uint8_t ultimoTiempo = 255;

/*****************************************
 * Code
 * **************************************/

int main(void){
	menuPrincipalActivo = true;
	modo = REFRIGERACION;

	NVIC_SetPriority(USART1_IRQn, 1);		// Alta prioridad para UART
	NVIC_SetPriority(I2C1_IRQn, 3);   		// Baja prioridad para I2C


	//Inicializaciones
	BOARD_InitDebugConsole();
	//Inicializo I2C
	I2C_Init();
	//Iniciacializo ADC
	InitADC((1 << 0) | (1 << 1)); // Habilita los canales 0 y 1
	//Inicializo OLED
	OLED_Init();
	OLED_Clear();
	OLED_Refresh();
	//Inicializo UART
	Init_UART();
	//Inicializo pulsadores y LED
	Init_PinIn();
	Init_PinOut();
	//Reinicio SHT30 para garantizar funcionamiento del modulo
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
	const tImage Advertencia = { image_data_Advertencia, 32, 32, 8 };
	const tImage Logo = { image_data_UTN, 61, 31, 8 };
	const tImage Opciones = { image_data_Opciones, 24, 24, 8 };


	//ShowIcon(UTN);//Arreglar porque se sigue viendo mal
	OLED_Draw_Logo(Logo.data, Logo.width, Logo.height, 25, 9);
	OLED_Refresh();
	SDK_DelayAtLeastUs(2000000, SystemCoreClock);

	ShowIconAndTextWithDelay(Opciones,"Seleccione Modo", 2000000);//con delay
	ShowIconAndText(Copodenieve,"Modo Refrigeracion");//sin delay

	while (1) {

		bool botonCambioActivo = leerApretarBotonCambio();
		if(botonCambioActivo){ //solo entro aca si toco el boton de cambio
			menuPrincipalActivo = true;
			modo = ((modo + 1) %3);

			switch(modo){
				case REFRIGERACION:
					ShowIconAndText(Copodenieve,"Modo Refrigeracion");
					break;
				case CALEFACCION:
					ShowIconAndText(Sol,"Modo Calefaccion");
					break;
				case TERMOHIGROMETRO:
					ShowIconAndText(Termometro,"Modo Termohigrometro");
					break;
				default:
					ShowIconAndText(Copodenieve,"Modo Refrigeracion");
			}
		}

		bool botonAceptacionActivo = leerApretarBotonAceptacion();
		if (botonAceptacionActivo){
			menuPrincipalActivo = false;
		}
		if(menuPrincipalActivo){
			continue;
		}

		if(modo == REFRIGERACION || modo == CALEFACCION){
			// Disparar la conversión del ADC
			ADC_StartConversion(ADC0);

			// Leer el resultado del canal 0 (Inyección)
			if (ADC_GetChannelResult(ADC0, 0, &adcResult0)) {
				inyeccion = ConvertADCToTemperature(adcResult0); // Convierte a temperatura
				//Verifico rango de temperatura y termistor defectuoso o faltante
				while (adcResult0 >= 3900 || adcResult0 <=60){
					if (adcResult0 >=3900){
						ShowIconAndText(Advertencia,"CONECTAR INYECCION");//sin delay
					}else if (adcResult0 <=60){
						ShowIconAndText(Advertencia,"REEMPLAZAR INYECCION");//sin delay
					}
					// Disparar la conversión del ADC
					ADC_StartConversion(ADC0);
					ADC_GetChannelResult(ADC0, 0, &adcResult0);
				}
			}

			// Leer el resultado del canal 1 (Retorno)
			if (ADC_GetChannelResult(ADC0, 1, &adcResult1)) {
				retorno = ConvertADCToTemperature(adcResult1); // Convierte a temperatura
				//Verifico rango de temperatura y termistor defectuoso o faltante
				while (adcResult1 >= 3900 || adcResult1 <=60){
					if (adcResult1 >=3900){
						ShowIconAndText(Advertencia,"CONECTAR RETORNO");//sin delay
					}else if (adcResult1 <=60){
						ShowIconAndText(Advertencia,"REEMPLAZAR RETORNO");//sin delay
					}
					// Disparar la conversión del ADC
					ADC_StartConversion(ADC0);
					ADC_GetChannelResult(ADC0, 1, &adcResult1);
				}
			}



			if (modo == REFRIGERACION){
				saltoTermico = retorno - inyeccion;
				prendeLEDRef(saltoTermico);
			}
			else{ //modo == CALEFACCION
				saltoTermico = inyeccion - retorno;
				prendeLEDCal(saltoTermico);
			}
		}

		if (modo == TERMOHIGROMETRO){
			GPIO_PinWrite(GPIO, 0, G_LED, 1); //Led Verde OFF
			GPIO_PinWrite(GPIO, 0, R_LED, 1); //Led Rojo OFF
			GPIO_PinWrite(GPIO, 0, Y_LED, 1); //Led Azul OFF
		}

		// Leer datos del SHT30
		if (SHT30_ReadData(I2C1_BASE, &data) == kStatus_Success) {
			data.dewpoint = SHT30_CalculateDewPoint(data.temperature, data.humidity);
		}

		// Obtener fecha y hora del RTC
		rtc_datetime_t datetime = GetRTCDateTime();

		// Enviar datos por UART en formato JSON
		if(ultimoTiempo != datetime.seconds){
			UpdateOLED(modo, inyeccion, retorno, saltoTermico, data);
			SendDataUART_JSON(modo, inyeccion, retorno, saltoTermico, data, datetime);
			ultimoTiempo = datetime.seconds;
		}


	}
}

