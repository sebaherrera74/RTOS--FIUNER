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

/** @file controlador.c
 **
 ** @brief Controlador de comunicaciones seriales universal
 **
 ** Ejemplo de la implementación de un controlador de comunicaciones seriales
 ** no blocante universar que puede utilizarse con cualquier sistema operativo
 ** o incluso sin sistema operativo
 ** 
 ** | RV | YYYY.MM.DD | Autor       | Descripción de los cambios              |
 ** |----|------------|-------------|-----------------------------------------|
 ** |  1 | 2017.09.16 | evolentini  | Version inicial del archivo             |
 ** 
 ** @defgroup ejemplos Proyectos de ejemplo
 ** @brief Proyectos de ejemplo de la Especialización en Sistemas Embebidos
 ** @{ 
 */

/* === Inclusiones de cabeceras ============================================ */
#include "controlador.h"
#include "uart.h"
#include <stdint.h>
#include <string.h>

/* === Definicion y Macros ================================================= */

/* === Declaraciones de tipos de datos internos ============================ */

/** @brief Estructura de datos para transmisión
 ** 
 ** Estructura que contiene los datos necesarios para que la interrupción 
 ** pueda continuar el envio de datos usando la función @ref EnviarCaracter
 ** de la transmisisión iniciada por la función @ref EnviarTexto.
 */
typedef struct {
   const char * datos;   /** < Puntero al bloque de datos a enviar */
   uint8_t cantidad;     /** < Cantidad de datos a enviar */
   uint8_t enviados;     /** < Cantidad de datos enviados */
} cola_t;

/* === Declaraciones de funciones internas ================================= */

/* === Definiciones de variables internas ================================== */

/** @brief Información para el envio de datos por la uart */
static cola_t cola;

/* === Definiciones de variables externas ================================== */

/* === Definiciones de funciones internas ================================== */

/* === Definiciones de funciones externas ================================== */

bool EnviarTexto(const char * cadena) {
   bool pendiente = FALSE;

   cola.datos = cadena;
   cola.cantidad = strlen(cadena);
   cola.enviados = 0;

   if (cola.cantidad) {
      Chip_UART_SendByte(USB_UART, cola.datos[cola.enviados]);
      cola.enviados++;

      if (cola.enviados < cola.cantidad) {
         Chip_UART_IntEnable(USB_UART, UART_IER_THREINT);
         pendiente = TRUE;
      }
   }
   return (pendiente);
}

void UART2_IRQHandler(void) {
   uint8_t eventos;

   eventos = Chip_UART_ReadLineStatus(USB_UART);

   if (eventos & UART_LSR_THRE) {
      Chip_UART_SendByte(USB_UART, cola.datos[cola.enviados]);
      cola.enviados++;

      if (cola.enviados == cola.cantidad) {
         Chip_UART_IntDisable(USB_UART, UART_IER_THREINT);
         EventoSerial();
      }
   }
}

__attribute__ ((weak)) void EventoSerial(void) {
}

/* === Ciere de documentacion ============================================== */

/** @} Final de la definición del modulo para doxygen */
