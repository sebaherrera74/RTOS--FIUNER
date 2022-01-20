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
 ** @brief Transmisión serial con interrupciones y FreeRTOS
 **
 ** Ejemplo de una transmisión serial utilizando interupciones y FreeRTOS. En 
 ** el mismo una tarea escanea el teclado y cuando se pulsa la tecla 1 se 
 ** resume una tarea que envia la cadena 'Hola' y una vez completa la transición
 ** de la misma se envia la cadena 'Mundo'. 
 ** 
 ** | RV | YYYY.MM.DD | Autor       | Descripción de los cambios              |
 ** |----|------------|-------------|-----------------------------------------|
 ** |  2 | 2017.10.21 | evolentini  | Correción en el formato del archivo     |
 ** |  1 | 2017.09.16 | evolentini  | Version inicial del archivo             |
 ** 
 ** @defgroup ejemplos Proyectos de ejemplo
 ** @brief Proyectos de ejemplo de la Especialización en Sistemas Embebidos
 ** @{ 
 */

/* === Inclusiones de cabeceras ============================================ */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "led.h"
#include "switch.h"
#include "controlador.h"
#include <stdint.h>
#include <string.h>

/* === Definicion y Macros ================================================= */

/** @briev Evento para inidicar que la transmisción esta completa */
#define EVENTO_COMPLETO   (1 << 0)

/* === Declaraciones de tipos de datos internos ============================ */

/* === Declaraciones de funciones internas ================================= */

/** @brief Tarea que escanea el teclado
 **
 ** Esta tarea se activa cada vez que expira la alarma RevisarTeclado, lee
 ** el estado actual de la teclas y lo compara con el de la ultima activacion
 ** para detectar los cambios en las teclas y generar eventos al detectar la
 ** pulsación de una tecla.
 */
void Teclado(void * parametros);

/** @brief Tarea que envia la cadena
 **
 ** Esta tarea se activa cada vez que presiona la tecla uno y transmite la
 ** cadena Hola y despues la cadena Mundo
 */
void Transmitir(void * parametros);

/* === Definiciones de variables internas ================================== */

/** @brief Descriptor de la tarea de envio para reactivarla */
TaskHandle_t enviar;

/** @brief Descriptor del grupo de eventos */
EventGroupHandle_t eventos;

/* === Definiciones de variables externas ================================== */

/* === Definiciones de funciones internas ================================== */

void Teclado(void * parametros) {
   uint8_t tecla;
   uint8_t anterior = 0;

   while(1) {
      tecla = Read_Switches();
      if (tecla != anterior) {
         switch(tecla) {
            case TECLA1:
                  // vTaskResume(xTaskGetHandle("Enviar"));
                  vTaskResume(enviar);
               break;
            case TECLA2:
               break;
            case TECLA3:
               break;
            case TECLA4
            :
               break;
         }
         anterior = tecla;
      }
      vTaskDelay(100);
      Led_Toggle(GREEN_LED);
   }
}

void Transmitir(void * parametros) {
   
   while(1) {

      vTaskSuspend(NULL);

      Led_On(YELLOW_LED);
      /* Espera que se transmita la primera cadena */
      if (EnviarTexto("Hola ")) {
         xEventGroupWaitBits(eventos, EVENTO_COMPLETO, TRUE, 
            FALSE, portMAX_DELAY);
      }

      /* Espera que se transmita la segunda cadena */
      if (EnviarTexto("Mundo\r\n")) {
         xEventGroupWaitBits(eventos, EVENTO_COMPLETO, TRUE, 
            FALSE, portMAX_DELAY);
      }

      Led_Off(YELLOW_LED);
   }
}

/* === Definiciones de funciones externas ================================== */

void EventoSerial(void) {
   BaseType_t planificar, resultado;
   planificar = pdFALSE;

   resultado = xEventGroupSetBitsFromISR(eventos, EVENTO_COMPLETO, &planificar);
   if(resultado == pdPASS) {
      portYIELD_FROM_ISR(planificar);
   }
}

/** @brief Función principal del programa
 **
 ** @returns 0 La función nunca debería termina
 **
 ** \remarks En un sistema embebido la función main() nunca debe terminar.
 **          El valor de retorno 0 es para evitar un error en el compilador.
 */
int main(void) {
   Board_Init();
   SisTick_Init();

   /* Inicializaciones y configuraciones de dispositivos */
   Init_Leds();
   Init_Switches();
   Init_Uart_Ftdi();

   NVIC_SetPriority(26, 7);
   NVIC_EnableIRQ(26);

   /* Creación del grupo de eventos */
   eventos = xEventGroupCreate();

   /* Creación de las tareas */
   if (eventos != NULL) {
      xTaskCreate(Teclado, "Teclado", configMINIMAL_STACK_SIZE, 
         NULL, tskIDLE_PRIORITY + 2, NULL);
      
      xTaskCreate(Transmitir, "Enviar", configMINIMAL_STACK_SIZE, 
         NULL, tskIDLE_PRIORITY + 1, &enviar);
      
      /* Arranque del sistema operativo */
      vTaskStartScheduler();
   }
   
   /* vTaskStartScheduler solo retorna si se detiene el sistema operativo */
   while(1);

   /* El valor de retorno es solo para evitar errores en el compilador*/
   return 0;
}
/* === Ciere de documentacion ============================================== */

/** @} Final de la definición del modulo para doxygen */
