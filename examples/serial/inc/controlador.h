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

#ifndef CONTROLADOR_H    /*! @cond    */
#define CONTROLADOR_H    /*! @endcond */

/** @file controlador.h
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

/* === Inclusiones de archivos externos ==================================== */
#include "FreeRTOS.h"

/* === Cabecera C++ ======================================================== */
#ifdef __cplusplus
extern "C" {
#endif

/* === Definicion y Macros ================================================= */

/* == Declaraciones de tipos de datos ====================================== */

/* === Declaraciones de variables externas ================================= */

/* === Declaraciones de funciones externas ================================= */

/** @brief Envio de una cadena por puerto serial
 **
 ** Esta función comienza el envio de una cadena serial por el puerto de la 
 ** uart conectada a la interface de depuracion USB. La función es no blocante
 ** y el resto de la cadena se envia mediante interupciones utilizando la
 ** función @ref EnviarCaracter en la rutina de servicio.
 ** 
 ** @param[in] cadena Puntero con la cadena de caracteres a enviar.
 ** @return Indica si quedan caracteres para enviar por interrupciones.
 */
bool EnviarTexto(const char * cadena);

/** @brief Envio de un caracter en una interrupcion.
 **
 ** Esta función envia un caracter por el puerto serie durante una rutina de
 ** servicio de interrupcion. La misma continua la transmisión inciada por la
 ** función @ref EnviarTexto. Cuando termina deshabilita las interrupciones
 ** del puerto serial y llama a una función de callback
 */
void UART2_IRQHandler(void);

/** @brief Rutina de servicio interrupcion serial
 **
 ** Esta rutina se activa cada vez que se vacia el buffer de transmisión de
 ** la uart y se encarga de enviar el siguiente caracter y si se completó la
 ** transmisión entonces notifica a la tarea con un evento.
 */
void EventoSerial(void);

/* === Ciere de documentacion ============================================== */
#ifdef __cplusplus
}
#endif

/** @} Final de la definición del modulo para doxygen */

#endif   /* CONTROLADOR_H */
