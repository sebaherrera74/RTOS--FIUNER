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
#include "soc.h"
#include "led.h"
#include "switch.h"

/* === Definicion y Macros ================================================= */

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



/* === Definiciones de variables internas ================================== */

bool detenido=true;
bool poneCero=true;
bool flagTiempo=true;
typedef struct {
	uint8_t decimas;
	uint8_t segundos;
	uint8_t minutos;
} tiempo_t;

tiempo_t tiempo;
tiempo_t tiempoParcial;
/* === Definiciones de variables externas ================================== */

/* === Definiciones de funciones internas ================================== */

void Blinking(void * parametros) {
	//blinking_t * valores = parametros;

	while(1) {
		if (detenido) {
			Led_On( RGB_R_LED);
			Led_Off(RGB_G_LED);
		}
		else{
			Led_Off(RGB_R_LED);
			Led_Toggle(RGB_G_LED);

		}
		vTaskDelay(500/ portTICK_PERIOD_MS);
	}
}

void Teclado(void * parametros)
{
	uint8_t actual;
	uint8_t anterior=0;
	while(1)
	{
		actual=Read_Switches();
		if (actual!=anterior)
		{
			if(( actual^anterior) & TECLA1)
			{
				if (actual & TECLA1){
					detenido=!detenido;  //Cambio el estado de la variable cuando presiono la tecla 1
					//Led_Toggle(RED_LED);
				}
				else
				{
					//Suelto tecla
				}
			}
			if(( actual^anterior) & TECLA2)
			{
				if (actual & TECLA2)
				{
					poneCero=!poneCero;  //Cambio el estado de la variable cuando presiono la tecla 1

				}
				else
				{
					//Suelto tecla
				}
			}
			if(( actual^anterior) & TECLA3)
			{
				if (actual & TECLA3)
				{
					flagTiempo=!flagTiempo;  //Cambio el estado de la variable cuando presiono la tecla 1

				}
				else
				{
					//Suelto tecla
				}
			}

			anterior=actual;
			vTaskDelay(100/ portTICK_PERIOD_MS);
		}

		vTaskDelay(50/ portTICK_PERIOD_MS);

	}
}
void Cronometro(void * parametros)
{
	TickType_t anterior;
	anterior=xTaskGetTickCount();

	while(1)
	{
		vTaskDelayUntil(&anterior,100/ portTICK_PERIOD_MS);

		if (!detenido)
		{
			tiempo.decimas++;

			if(tiempo.decimas>=10){
				tiempo.decimas=0;
				tiempo.segundos++;
			}
			if(tiempo.segundos >=60){
				tiempo.segundos=0;
				tiempo.minutos++;
			}
			if (tiempo.minutos==60){
				tiempo.minutos=0;

			}
		}
		else{
			if(!poneCero){

				tiempo.decimas=0;
				tiempo.segundos=0;
				tiempo.minutos=0;
				poneCero=true;
			}

		}

		if(!flagTiempo){
			tiempoParcial.decimas=tiempo.decimas;
			tiempoParcial.segundos=tiempo.segundos;
			tiempoParcial.minutos=tiempo.minutos;
			flagTiempo=true;

		}
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
int main(void) {


	/* Inicializaciones y configuraciones de dispositivos */

	SisTick_Init();
	Init_Leds();

	/* Creación de las tareas */
	xTaskCreate(Blinking, "Toggle", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(Teclado, "Teclas", configMINIMAL_STACK_SIZE,NULL, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(Cronometro, "Cronometros", configMINIMAL_STACK_SIZE,NULL, tskIDLE_PRIORITY + 1, NULL);

	/* Arranque del sistema operativo */
	vTaskStartScheduler();

	/* vTaskStartScheduler solo retorna si se detiene el sistema operativo */
	while(1);

	/* El valor de retorno es solo para evitar errores en el compilador*/
	return 0;
}
/* === Ciere de documentacion ============================================== */
/** @} Final de la definición del modulo para doxygen */
