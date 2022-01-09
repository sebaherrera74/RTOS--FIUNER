/* Copyright 2017, Esteban Volentini - Facet UNT, Fi UNER
 * Copyright 2014, 2015 Mariano Cerdeiro
 * Copyright 2014, Pablo Ridolfi
 * Copyright 2014, Juan Cecconi
 * Copyright 2014, Gustavo Muro
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

///----------Requerimientos del Trabajo-------------------
/*---El botón TEC1 debe iniciar y detener la cuenta.
-----El botón TEC2 debe volver la cuenta a cero solo si esta detenido.
-----Mientras la cuenta esta corriendo deben parpadear el led RGB en
     verde.
-----Mientras la cuenta esta detenida el led RGB debe permanecer en rojo.
-----Si el cronometro esta funcionando y la cuenta se muestra en la
     pantalla la tecla TEC3 debe congelar el tiempo parcial y mantener la
     cuenta activa.
-----Si se esta mostrando un tiempo parcial la tecla TEC3 debe volver a
     mostrar la cuenta actual de cronometro.*/

/** @file blinking.c
 **
 ** @brief Ejemplo de un led parpadeando
 **
 ** Ejemplo de un led parpadeando utilizando la capa de abstraccion de 
 ** hardware y con sistema operativo FreeRTOS.
 ** 
 ** | RV | YYYY.MM.DD | Autor       | Descripción de los cambios              |
 ** |----|------------|-------------|-----------------------------------------|
 ** |  2 | 2017.10.16 | evolentini  | Correción en el formato del archivo     |
 ** |  1 | 2017.09.21 | evolentini  | Version inicial del archivo             |
 ** 
 ** @defgroup ejemplos Proyectos de ejemplo
 ** @brief Proyectos de ejemplo de la Especialización en Sistemas Embebidos
 ** @{ 
 */

/* === Inclusiones de cabeceras ============================================ */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "soc.h"
#include "led.h"
#include "switch.h"
#include "chip.h"
#include "ili9341.h"
#include "event_groups.h"
#include <string.h>
#include <stdio.h>

/* === Definicion y Macros ================================================= */
#define SPI_1   1  /*!< EDU-CIAA SPI port */
#define GPIO_0  0 /*!< EDU-CIAA GPIO0 port */
#define GPIO_6  6 /*!< EDU-CIAA GPIO1 port */
#define GPIO_7  7 /*!< EDU-CIAA GPIO2 port */
#define EVENTO_TECLA_1_ON 	( 1 << 0 )
#define EVENTO_TECLA_2_ON 	( 1 << 1 )
#define EVENTO_TECLA_3_ON 	( 1 << 2 )
#define EVENTO_TECLA_4_ON 	( 1 << 3 )
#define EVENTO_TECLA_1_OFF ( 1 << 4 )
#define EVENTO_TECLA_2_OFF ( 1 << 5 )
#define EVENTO_TECLA_3_OFF ( 1 << 6 )
#define EVENTO_TECLA_4_OFF ( 1 << 7 )
/* === Declaraciones de tipos de datos internos ============================ */

/* === Declaraciones de funciones internas ================================= */

/** @brief Función que implementa una tarea de baliza
 ** 
 ** @parameter[in] parametros Puntero a una estructura que contiene el led
 **                           y la demora entre encendido y apagado.
 */ 
void Blinking(void * parametros);
void Teclado(void * parametros);
void Cronometro(void * parametros);
void PuestaCero(void * parametros);
void Display(void * parametros);

/* === Definiciones de variables internas ================================== */

typedef struct tiempo_s {
	uint8_t decimas;
	uint8_t segundos;
	uint8_t minutos;
}tiempo_t;



SemaphoreHandle_t mutex;
EventGroupHandle_t eventos;

//tiempo_t tiempo;

/* === Definiciones de variables externas ================================== */

/* === Definiciones de funciones internas ================================== */

void Blinking(void * parametros) {
	//blinking_t * valores = parametros;
	EventBits_t uxBits;

	while(1) {
		uxBits=xEventGroupWaitBits(eventos, EVENTO_TECLA_1_ON, pdFALSE, pdTRUE,100/ portTICK_PERIOD_MS);
		if(	uxBits)
		{
			Led_Toggle(RGB_R_LED);
			Led_Off(RGB_B_LED);
		}

		else{
			Led_Off(RGB_R_LED);
			Led_On(RGB_B_LED);

		}
		vTaskDelay(300/ portTICK_PERIOD_MS);
	}
}

void Teclado(void * parametros)
{
	uint8_t actual;
	uint8_t anterior=0;
	uint32_t cambios;
	uint32_t contador=0;
	while(1)
	{
		actual=Read_Switches();
		cambios=actual^anterior;

		if(cambios & TECLA1)
		{
			if (actual & TECLA1){
				xEventGroupSetBits(eventos, EVENTO_TECLA_1_ON);
				contador++;
				if ((contador%2)==0){
					xEventGroupClearBits (eventos,  EVENTO_TECLA_1_ON);
				}
			}

			/*else
				{
					xEventGroupSetBits(eventos, EVENTO_TECLA_1_OFF);
				}*/
		}
		if(cambios & TECLA2)
		{
			if (actual & TECLA2)
			{
				if ((contador%2)==0){
					xEventGroupSetBits(eventos, EVENTO_TECLA_2_ON);
				}
			}
			else
			{
				xEventGroupSetBits(eventos, EVENTO_TECLA_2_OFF);
			}
		}
		if(cambios & TECLA3)
		{
			if (actual & TECLA3)
			{
				xEventGroupSetBits(eventos, EVENTO_TECLA_3_ON);
			}
			else
			{
				xEventGroupSetBits(eventos, EVENTO_TECLA_3_OFF);
			}
		}
		anterior=actual;
		vTaskDelay(100/ portTICK_PERIOD_MS);
	}
}
void Cronometro(void * parametros)
{
	tiempo_t * argumentos=(tiempo_t *)parametros;
	TickType_t anterior;
	anterior=xTaskGetTickCount();

	while(1)
	{
		vTaskDelayUntil(&anterior,100/ portTICK_PERIOD_MS);
		xEventGroupWaitBits(eventos, EVENTO_TECLA_1_ON, pdFALSE, pdTRUE,portMAX_DELAY);
		xSemaphoreTake(mutex,portMAX_DELAY);
		argumentos->decimas++;
		//Led_Toggle(RGB_G_LED);
		if(argumentos->decimas>=10){
			argumentos->decimas=0;
			argumentos->segundos++;
		}
		if(argumentos->segundos>=60){
			argumentos->segundos=0;
			argumentos->minutos++;
		}
		if (argumentos->minutos==60){
			argumentos->minutos=0;
		}
		anterior=xTaskGetTickCount();
		xSemaphoreGive(mutex);

	}
}

void PuestaCero(void * parametros)
{
	tiempo_t *argumentos=(tiempo_t *)parametros;


	while(1)
	{
		xEventGroupWaitBits(eventos, EVENTO_TECLA_2_ON, pdTRUE, pdTRUE,portMAX_DELAY);
		argumentos->decimas=0;
		argumentos->segundos=0;
		argumentos->minutos=0;
	}
}

void Display(void * parametros)
{

	tiempo_t *argumentos=(tiempo_t*)parametros;

	static char muestrahora[9];
	char muestrahoraparcial[9];
	while(1) {
		//xSemaphoreTake(mutex,portMAX_DELAY);
		sprintf(muestrahora,"%02d:%02d:%02d",argumentos->minutos,argumentos->segundos,argumentos->decimas);
		//sprintf(muestrahoraparcial,"%02d:%02d:%02d",tiempoParcial.minutos,tiempoParcial.segundos,tiempoParcial.decimas);
		ILI9341DrawString(100, 25, muestrahora, &font_16x26, ILI9341_BLACK, ILI9341_WHITE);
		//xSemaphoreGive(mutex);
		//ILI9341DrawString(100, 140, muestrahoraparcial, &font_16x26, ILI9341_BLACK, ILI9341_WHITE);

		vTaskDelay(100/ portTICK_PERIOD_MS);
	}



}


/* === Definiciones de funciones externas ================================== */

/** @brief Función principal del programa
 **
 ** @returns 0 La función nunca debería termina
 **
 ** @remarks En un sistema embebido la función main() nunca debe terminar.
 **          El valor de retorno 0 es para evitar un error en el compilador.
 */
int main(void)
{
	tiempo_t tiempo={.decimas=0,.segundos=0,.minutos=0};

	/* Inicializaciones y configuraciones de dispositivos */
	Init_Leds();
	ILI9341Init(SPI_1, GPIO_0, GPIO_6, GPIO_7);
	ILI9341Rotate(ILI9341_Landscape_1);
	//ILI9341DrawString(50, 25, horaprueba, &font_16x26, ILI9341_BLACK, ILI9341_WHITE);
	SisTick_Init();


	mutex=xSemaphoreCreateMutex();
	eventos=xEventGroupCreate();
	/* Creación de las tareas */
	xTaskCreate(Blinking,  "Toggle", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(Teclado,   "Teclas", configMINIMAL_STACK_SIZE,NULL, tskIDLE_PRIORITY + 3, NULL);
	xTaskCreate(Cronometro,"Cronometros", configMINIMAL_STACK_SIZE,(void*)&tiempo, tskIDLE_PRIORITY + 2,NULL);
	xTaskCreate(PuestaCero,"Puesta a cero ", configMINIMAL_STACK_SIZE,(void*)&tiempo, tskIDLE_PRIORITY + 1,NULL);
	xTaskCreate(Display,"Display", configMINIMAL_STACK_SIZE*4,(void*)&tiempo, tskIDLE_PRIORITY + 4, NULL);

	/* Arranque del sistema operativo */
	vTaskStartScheduler();

	/* vTaskStartScheduler solo retorna si se detiene el sistema operativo */
	while(1);

	/* El valor de retorno es solo para evitar errores en el compilador*/
	return 0;
}
/* === Ciere de documentacion ============================================== */
/** @} Final de la definición del modulo para doxygen */
