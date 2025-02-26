/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*----esta modificada para usar el I2C1, en caso de utilizar otro cambiar todo lo que diga I2C1 por la base del I2C usado--- */
/*---- BASADO EN fsl_SSD1306.h para SPI de NXP ------*/

#include "../Cabecera/OLED_SSD1306_I2C.h"
#include "../Cabecera/Iconos.h"
#include "fsl_gpio.h"
#include "fsl_i2c.h"
//#include "fsl_Systick_Delay.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief OLED buffer */
static uint8_t OLED_Buffer[(OLED_WIDTH * OLED_HEIGHT) / 8];
char bufferOLED[64]; // Buffer para texto en la pantalla OLED


/*******************************************************************************
 * Code
 ******************************************************************************/

static void OLED_Command (uint8_t Cmd)
{
	i2c_master_transfer_t xfer = {0};

	xfer.data = (uint8_t *)&Cmd;
	xfer.dataSize = sizeof(Cmd);
	xfer.flags = kI2C_TransferDefaultFlag;
	xfer.slaveAddress = SSD1306_ADDRESS_1;
	xfer.direction = kI2C_Write;
	xfer.subaddress = 0x0;
	xfer.subaddressSize = 1;

	I2C_MasterTransferBlocking(I2C1, &xfer);

}

static void OLED_Data (uint8_t *Data)
{
	i2c_master_transfer_t xfer = {0};

	/*Start Transfer*/
	xfer.data = Data;
	xfer.dataSize = sizeof(OLED_Buffer);
	xfer.flags = kI2C_TransferDefaultFlag;
	xfer.slaveAddress = SSD1306_ADDRESS_1;
	xfer.direction = kI2C_Write;
	xfer.subaddress = 0x40;
	xfer.subaddressSize = 1;

	I2C_MasterTransferBlocking(I2C1, &xfer);

}

static void OLED_Reset (void)
{
	OLED_Command(OLED_DISPLAYON);
//	for(int i=0; i<1000; i++);					//delay 1ms
	OLED_Command(OLED_DISPLAYOFF);
//	for(int i=0; i<1000; i++);					//delay 1ms
	OLED_Command(OLED_DISPLAYON);
//	for(int i=0; i<10000; i++);					//delay 10ms

}

static void OLED_Config_Display (void)
{

	OLED_Reset();

	// Turn the OLED Display off
	OLED_Command(OLED_DISPLAYOFF);

	// Configure the display for 128x64 pixels, KS0108 mode
	OLED_Command(OLED_SETDISPLAYCLOCKDIV);
	OLED_Command(0x80);
	OLED_Command(OLED_SETMULTIPLEX);
	OLED_Command(OLED_HEIGHT-1);				// LCD Height
	OLED_Command(OLED_SETDISPLAYOFFSET);
	OLED_Command(0x0);
	OLED_Command(OLED_SETSTARTLINE | 0x0);
	OLED_Command(OLED_CHARGEPUMP);
	OLED_Command(0x14);							// Use 3.3V supply to generate high voltage supply
	OLED_Command(OLED_MEMORYMODE);
	OLED_Command(0x00);
	OLED_Command(OLED_SEGREMAP | 0x1);
	OLED_Command(OLED_COMSCANDEC);
	OLED_Command(OLED_SETCOMPINS);
	OLED_Command(0x12);
	OLED_Command(OLED_SETCONTRAST);
	OLED_Command(0xCF);
	OLED_Command(OLED_SETPRECHARGE);
	OLED_Command(0xF1);
	OLED_Command(OLED_SETVCOMDETECT);
	OLED_Command(0x40);
	OLED_Command(OLED_DISPLAYALLON_RESUME);
	OLED_Command(OLED_NORMALDISPLAY);
	OLED_Command(OLED_DEACTIVATE_SCROLL);

	OLED_Command(OLED_COLUMNADDR);
	OLED_Command(0);
	OLED_Command(OLED_WIDTH-1);
	OLED_Command(OLED_PAGEADDR);
	OLED_Command(0);
	OLED_Command(OLED_HEIGHT/8-1);

	// Turn the OLED display on!
	OLED_Command(OLED_DISPLAYON);

	OLED_Command(OLED_SETLOWCOLUMN | 0x0);  // low col = 0
	OLED_Command(OLED_SETHIGHCOLUMN | 0x0);  // hi col = 0
	OLED_Command(OLED_SETSTARTLINE | 0x0); // line #0

}


static int OLED_Render_Char (uint8_t X_axis, uint8_t Y_axis, uint8_t SC, int8_t String, uint8_t Scale)
{

	uint8_t px, py;
	uint16_t start_pos;

	if ((X_axis >= OLED_WIDTH) || (Y_axis >= OLED_HEIGHT)) {
		return 1;
	}
	if (String > 127) {
		return 2;
	}
	if (Scale > 3) {
		return 3;
	}

	start_pos = ((uint8_t)String) * 7;			// Characters have a 7 row offset
	for (px=0; px<5; px++) {
		for (py=0; py<7; py++) {
			if ((font5x7[start_pos+py] >> (7-px)) & 1) {
				switch (Scale) {
				case 3:
					OLED_Set_Pixel(X_axis+(px*Scale),   Y_axis+(py*Scale),  SC);
					OLED_Set_Pixel(X_axis+(px*Scale)+1, Y_axis+(py*Scale),  SC);
					OLED_Set_Pixel(X_axis+(px*Scale)+2, Y_axis+(py*Scale),  SC);
					OLED_Set_Pixel(X_axis+(px*Scale),   Y_axis+(py*Scale)+1, SC);
					OLED_Set_Pixel(X_axis+(px*Scale)+1, Y_axis+(py*Scale)+1, SC);
					OLED_Set_Pixel(X_axis+(px*Scale)+2, Y_axis+(py*Scale)+1, SC);
					OLED_Set_Pixel(X_axis+(px*Scale),   Y_axis+(py*Scale)+2, SC);
					OLED_Set_Pixel(X_axis+(px*Scale)+1, Y_axis+(py*Scale)+2, SC);
					OLED_Set_Pixel(X_axis+(px*Scale)+2, Y_axis+(py*Scale)+2, SC);
					break;
				case 2:
					OLED_Set_Pixel(X_axis+(px*Scale),   Y_axis+(py*Scale),  SC);
					OLED_Set_Pixel(X_axis+(px*Scale)+1, Y_axis+(py*Scale),  SC);
					OLED_Set_Pixel(X_axis+(px*Scale),   Y_axis+(py*Scale)+1, SC);
					OLED_Set_Pixel(X_axis+(px*Scale)+1, Y_axis+(py*Scale)+1, SC);
					break;
				case 1:
				default:
					OLED_Set_Pixel(X_axis+px, Y_axis+py, SC);
					break;
				}
			}
		}
	}
	return 0;
}

void OLED_Init(void)
{

	/*Give the display a reset*/
	OLED_Reset();

	/* Clear the framebuffer*/
	OLED_Clear();

	/*Configure the OLED display controller*/
	OLED_Config_Display();

}

void OLED_Refresh(void)
{

	OLED_Command(0xb0);
	OLED_Command(((0&0xf0)>>4) | 0x10);
	OLED_Command((0&0x0f) | 0x01);

	OLED_Data(&OLED_Buffer[0]);

}

void OLED_Clear(void)
{

	memset(OLED_Buffer, 0, sizeof(OLED_Buffer));

}


void OLED_Fill(uint8_t Pattern)
{

	memset(OLED_Buffer, Pattern, sizeof(OLED_Buffer));

}

void OLED_Display_Mode (uint8_t Mode)
{

	if (Mode) {
		OLED_Command(OLED_INVERTDISPLAY);
	}
	else {
		OLED_Command(OLED_NORMALDISPLAY);
	}

}

void OLED_Set_Pixel (uint8_t X_axis, uint8_t Y_axis, uint8_t SC)
{

	if((X_axis >= OLED_WIDTH) || (Y_axis >= OLED_HEIGHT))
	{
		//Do nothing
	}
	else
	{
		switch(SC)
		{
			case kOLED_Pixel_Clear:
				OLED_Buffer [X_axis + (Y_axis / 8) * OLED_WIDTH] &= ~(1 << (Y_axis & 7));
				break;
			case kOLED_Pixel_Set:
				OLED_Buffer [X_axis + (Y_axis / 8) * OLED_WIDTH] |= (1 << (Y_axis & 7));
				break;
		}
	}
}

void OLED_Set_Text (uint8_t X_axis, uint8_t Y_axis, uint8_t SC, char* String, uint8_t Scale)
{
	uint16_t Cont;
	uint16_t xscaled;

	if ((X_axis >= OLED_WIDTH) || (Y_axis >= OLED_HEIGHT)) {
		//Do nothing
	}
	else
	{
		if (Scale > 3) {
			//Do nothing
		}
		else
		{

			for (Cont = 0; String[Cont] != '\0'; Cont++) {
				// Catch overflow when scaling!
				xscaled = X_axis+(Cont*5*Scale);
				if (xscaled > OLED_WIDTH)
				{
					//Do nothing
				}

				else
				{
					OLED_Render_Char(xscaled, Y_axis, SC, String[Cont], Scale);

				}
			}
		}
	}

}

void OLED_Copy_Image(const uint8_t *Img, uint16_t size){

	uint16_t CpyBuffer;

	OLED_Clear();

	for(CpyBuffer = 0; CpyBuffer < size - 1; CpyBuffer++)
	{
		OLED_Buffer[CpyBuffer] = *(Img + CpyBuffer);
	}
}

void OLED_Draw_Logo(const uint8_t *bitmap, uint8_t width, uint8_t height, uint8_t x, uint8_t y)
{
    // Calcula los bytes por fila (ancho en píxeles dividido por 8, redondeado hacia arriba)
    uint8_t bytes_per_row = (width + 7) / 8;

    for (uint8_t i = 0; i < height; i++) // Filas
    {
        for (uint8_t j = 0; j < width; j++) // Columnas
        {
            // Calcula el índice del byte en el array del bitmap
            uint8_t byte_index = (i * bytes_per_row) + (j / 8);
            uint8_t bit_index = 7 - (j % 8); // Orden de los bits: MSB a LSB

            // Determina si el píxel está encendido
            uint8_t pixel = (bitmap[byte_index] >> bit_index) & 0x01;

            // Dibuja el píxel en el buffer
            OLED_Set_Pixel(x + j, y + i, pixel ? kOLED_Pixel_Set : kOLED_Pixel_Clear);
        }
    }

    // Refresca el display para aplicar los cambios (opcional)
    // OLED_Refresh();
}


void OLED_Fill_Rect(uint8_t X_axis, uint8_t Y_axis, uint8_t Width, uint8_t Height, uint8_t SC)
{
    // Asegúrate de que las coordenadas estén dentro de los límites del display
    if (X_axis >= OLED_WIDTH || Y_axis >= OLED_HEIGHT)
    {
        return; // Salir si está fuera de los límites
    }

    // Limita el ancho y alto al tamaño máximo del display
    if (X_axis + Width > OLED_WIDTH)
    {
        Width = OLED_WIDTH - X_axis;
    }

    if (Y_axis + Height > OLED_HEIGHT)
    {
        Height = OLED_HEIGHT - Y_axis;
    }

    // Recorrer las filas y columnas dentro de la región especificada
    for (uint8_t y = Y_axis; y < Y_axis + Height; y++)
    {
        for (uint8_t x = X_axis; x < X_axis + Width; x++)
        {
            // Llamar a OLED_Set_Pixel para establecer o limpiar los píxeles
            OLED_Set_Pixel(x, y, SC);
        }
    }
}


/************************************
 * Uso especifico para este proyecto
 * **********************************/

void UpdateOLED(Modo modo,float inyeccion, float retorno, float saltoTermico, sht30_data_t data) {
    OLED_Clear(); // Limpia el buffer del OLED para preparar la nueva información

    if (modo == TERMOHIGROMETRO){
    	// Línea 1: Título
    	OLED_Set_Text(0, 0, kOLED_Pixel_Set, "TERMOHIGROMETRO", 1);

    	// Línea 2: Datos del SHT30 - Temp Ref
    	sprintf(bufferOLED, "Temp Ref: %0.2f C", data.temperature);
    	OLED_Set_Text(0, 20, kOLED_Pixel_Set, bufferOLED, 1); // Línea en Y = 32

    	// Línea 3: Datos del SHT30 - Hum Ref
    	sprintf(bufferOLED, "Hum Ref: %d%%", (int)data.humidity);
    	OLED_Set_Text(0, 28, kOLED_Pixel_Set, bufferOLED, 1); // Línea en Y = 40

    	// Línea 4: Datos del SHT30 - DewPoint
    	sprintf(bufferOLED, "Dew Point: %0.2f C", data.dewpoint);
    	OLED_Set_Text(0, 36, kOLED_Pixel_Set, bufferOLED, 1); // Línea en Y = 48


    }else if (modo == REFRIGERACION){
    	// Línea 1: Título
    	OLED_Set_Text(0, 0, kOLED_Pixel_Set, "REFRIGERACION", 1);

    	// Línea 2: Datos de Inyección
    	sprintf(bufferOLED, "Inyeccion: %.2f C", inyeccion);
    	OLED_Set_Text(0, 8, kOLED_Pixel_Set, bufferOLED, 1); // Línea en Y = 8

    	// Línea 3: Datos de Retorno
    	sprintf(bufferOLED, "Retorno: %.2f C", retorno);
    	OLED_Set_Text(0, 16, kOLED_Pixel_Set, bufferOLED, 1); // Línea en Y = 16

    	// Línea 4: Salto Térmico
    	sprintf(bufferOLED, "Salto Termico: %.2f C", saltoTermico);
    	OLED_Set_Text(0, 24, kOLED_Pixel_Set, bufferOLED, 1); // Línea en Y = 24

    	// Línea 5: Datos del SHT30 - Temp Ref
    	sprintf(bufferOLED, "Temp Ref: %0.2f C", data.temperature);
    	OLED_Set_Text(0, 32, kOLED_Pixel_Set, bufferOLED, 1); // Línea en Y = 32

    	// Línea 6: Datos del SHT30 - Hum Ref
    	sprintf(bufferOLED, "Hum Ref: %d%%", (int)data.humidity);
    	OLED_Set_Text(0, 40, kOLED_Pixel_Set, bufferOLED, 1); // Línea en Y = 40

    	// Línea 7: Datos del SHT30 - DewPoint
    	sprintf(bufferOLED, "Dew Point: %0.2f C", data.dewpoint);
    	OLED_Set_Text(0, 48, kOLED_Pixel_Set, bufferOLED, 1); // Línea en Y = 48

    }else{//CALEFACCION
    	// Línea 1: Título
    	OLED_Set_Text(0, 0, kOLED_Pixel_Set, "CALEFACCION", 1);

    	// Línea 2: Datos de Inyección
    	sprintf(bufferOLED, "Inyeccion: %.2f C", inyeccion);
    	OLED_Set_Text(0, 8, kOLED_Pixel_Set, bufferOLED, 1); // Línea en Y = 8

    	// Línea 3: Datos de Retorno
    	sprintf(bufferOLED, "Retorno: %.2f C", retorno);
    	OLED_Set_Text(0, 16, kOLED_Pixel_Set, bufferOLED, 1); // Línea en Y = 16

    	// Línea 4: Salto Térmico
    	sprintf(bufferOLED, "Salto Termico: %.2f C", saltoTermico);
    	OLED_Set_Text(0, 24, kOLED_Pixel_Set, bufferOLED, 1); // Línea en Y = 24

    	// Línea 5: Datos del SHT30 - Temp Ref
    	sprintf(bufferOLED, "Temp Ref: %0.2f C", data.temperature);
    	OLED_Set_Text(0, 32, kOLED_Pixel_Set, bufferOLED, 1); // Línea en Y = 32

    	// Línea 6: Datos del SHT30 - Hum Ref
    	sprintf(bufferOLED, "Hum Ref: %d%%", (int)data.humidity);
    	OLED_Set_Text(0, 40, kOLED_Pixel_Set, bufferOLED, 1); // Línea en Y = 40

    	// Línea 7: Datos del SHT30 - DewPoint
    	sprintf(bufferOLED, "Dew Point: %0.2f C", data.dewpoint);
    	OLED_Set_Text(0, 48, kOLED_Pixel_Set, bufferOLED, 1); // Línea en Y = 48

    }

    OLED_Refresh(); // Refresca el OLED para mostrar los nuevos datos
}

/*Paso el texto en formato "Texto a mostrar" y va a apatecer centrado
 *en la pantalla
 */
void ShowText(const char *text) {
    uint8_t textLength = strlen(text);                         // Longitud del texto
    uint8_t textWidth = textLength * 5;                        // Ancho del texto sin escala (cada carácter = 5 px)
    uint8_t scale = 1;                                         // Escala del texto (puedes cambiarla entre 1, 2 o 3)
    uint8_t x = (OLED_WIDTH - (textWidth * scale)) / 2;        // Centrado horizontal
    uint8_t y = (OLED_HEIGHT - (8 * scale)) / 1.4;               // Centrado vertical (altura del texto = 8 px)

    OLED_Set_Text(x, y, 1, (char *)text, scale);               // Dibujar el texto centrado

    return;
}




void ShowIcon(tImage logo) {
    // Dibuja el ícono en la pantalla OLED
    uint8_t x = (OLED_WIDTH - logo.width) / 2;
    uint8_t y = (OLED_HEIGHT - logo.height) / 4;

    OLED_Clear(); // Limpiar la pantalla
    OLED_Draw_Logo(logo.data, logo.width, logo.height, x, y);

    return;
}


void ShowIconAndText(tImage logo, const char *text){
	OLED_Clear();

	ShowIcon(logo);
	ShowText(text);

	OLED_Refresh();

	return;
}

void ShowIconAndTextWithDelay(tImage logo, const char *text, uint32_t delay){
	OLED_Clear();

	ShowIcon(logo);
	ShowText(text);

	OLED_Refresh();

	SDK_DelayAtLeastUs(delay, SystemCoreClock);

	return;
}



