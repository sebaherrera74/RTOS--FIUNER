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
#include "soc.h"
#include "led.h"
#include  "semphr.h"
#include "switch.h"
#include "event_groups.h"
/* === Definicion y Macros ================================================= */
#define EVENTO_TECLA_1_ON 	( 1 << 0 )
#define EVENTO_TECLA_2_ON 	( 1 << 1 )
#define EVENTO_TECLA_3_ON 	( 1 << 2 )
#define EVENTO_TECLA_4_ON 	( 1 << 3 )
#define EVENTO_TECLA_1_OFF ( 1 << 4 )
#define EVENTO_TECLA_2_OFF ( 1 << 5 )
#define EVENTO_TECLA_3_OFF ( 1 << 6 )
#define EVENTO_TECLA_4_OFF ( 1 << 7 )

/* === Declaraciones de tipos de datos internos ============================ */
/*Estructura para pasar por parametros a la tarea de prende y apaga led*/
typedef struct {
	uint8_t led ;	/* * < Led que debe parpadear la tarea */
	uint16_t delay ; /* * < Demora entre cada encendido*/
}blinking_t;




/* === Declaraciones de funciones internas ================================= */

/** @brief Función que implementa una tarea de baliza
 ** 
 ** @parameter[in] parametros Puntero a una estructura que contiene el led
 **                           y la demora entre encendido y apagado.
 */ 
void Blinking(void * parametros);
void Teclas(void * parametros);

/* === Definiciones de variables internas ================================== */

SemaphoreHandle_t mutex;
EventGroupHandle_t eventos;

/* === Definiciones de variables externas ================================== */

/* === Definiciones de funciones internas ================================== */

void Blinking(void * parametros) {
	blinking_t * valores = parametros;
	EventBits_t uxBits;
	while(1) {

		uxBits=xEventGroupWaitBits(eventos, EVENTO_TECLA_1_ON, pdTRUE, pdFALSE, portMAX_DELAY);
		xSemaphoreTake(mutex,portMAX_DELAY);
		//Led_Toggle(valores->led);
		Led_On(valores->led);
		vTaskDelay(valores->delay/ portTICK_PERIOD_MS);
		Led_Off(valores->led);
		xSemaphoreGive(mutex);
		vTaskDelay(valores->delay/ portTICK_PERIOD_MS);
		//vTaskDelay(1/ portTICK_PERIOD_MS);
	}
}


void Teclas(void * parametros){
	uint8_t anterior = 0;
	uint8_t actual;
	uint8_t cambios;
	while(1){
		actual = Read_Switches();
		cambios = actual ^ anterior;

		if (cambios & TECLA1){
			if (actual & TECLA1){
				xEventGroupSetBits(eventos, EVENTO_TECLA_1_ON);
			} else {
				xEventGroupSetBits(eventos, EVENTO_TECLA_1_OFF);
			}
		}
		if (cambios & TECLA2){
			if (actual & TECLA2){
				xEventGroupSetBits(eventos, EVENTO_TECLA_2_ON);
			} else {
				xEventGroupSetBits(eventos, EVENTO_TECLA_2_OFF);
			}
		}
		if (cambios & TECLA3){
			if (actual & TECLA3){
				xEventGroupSetBits(eventos, EVENTO_TECLA_3_ON);
			} else {
				xEventGroupSetBits(eventos, EVENTO_TECLA_3_OFF);
			}
		}
		if (cambios & TECLA4){
			if (actual & TECLA4){
				xEventGroupSetBits(eventos, EVENTO_TECLA_4_ON);
			} else {
				xEventGroupSetBits(eventos, EVENTO_TECLA_4_OFF);
			}
		}
		anterior = actual;
		vTaskDelay(pdMS_TO_TICKS(150));
	}}

	/* === Definiciones de funciones externas ================================== */

	/** @brief Función principal del programa
	 **
	 ** @returns 0 La función nunca debería termina
	 **
	 ** @remarks En un sistema embebido la función main() nunca debe terminar.
	 **          El valor de retorno 0 es para evitar un error en el compilador.
	 */
	int main(void) {

		/* Variable con los parametros de las tareas */
		static blinking_t valores[]={
				{.led =RGB_B_LED, .delay = 500},
				{.led =RGB_R_LED, .delay =300}
		};


		mutex=xSemaphoreCreateMutex();
        eventos=xEventGroupCreate();

		/* Inicializaciones y configuraciones de dispositivos */

		SisTick_Init();
		Init_Leds();

		/* Creación de las tareas */
		xTaskCreate(Blinking, "Azul", configMINIMAL_STACK_SIZE, &valores[0], tskIDLE_PRIORITY + 1, NULL);
		//xTaskCreate(Blinking, "Rojo", configMINIMAL_STACK_SIZE, &valores[1], tskIDLE_PRIORITY + 1, NULL);
		xTaskCreate(Teclas, "Teclas", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
		/* Arranque del sistema operativo */
		vTaskStartScheduler();

		/* vTaskStartScheduler solo retorna si se detiene el sistema operativo */
		while(1);

		/* El valor de retorno es solo para evitar errores en el compilador*/
		return 0;
	}
	/* === Ciere de documentacion ============================================== */
	/** @} Final de la definición del modulo para doxygen */
