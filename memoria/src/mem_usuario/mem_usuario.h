#ifndef MEM_USUARIO_H
#define MEM_USUARIO_H

#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include <protocol/protocol.h>

#include "config/config.h"
#include "estado_frame/estado_frame.h"
#include "tabla_paginas/tabla_paginas.h"

/**
 * @brief Función constructor de la memoria de usuario.
 *        Inicializa la memoria de usuario con el espacio
 *        especificado por el archivo de configuración.
 */
void inicializar_memoria_usuario(void);

/**
 * @brief Función llamada ante una petición de `RESIZE`.
 *        Evalúa el tamaño solicitado respecto del actual y,
 *        amplía o reduce según resultado.
 *
 * @param pid
 * @param tamanio
 * @return `int8_t` : 0 si se pudo ajustar, -1 en caso de error.
 */
int8_t ajustar_memoria_para_proceso(u_int32_t pid, u_int32_t tamanio);

/**
 * @brief Amplia memoria para un proceso.
 *        Si la cantidad de memoria solicitada excede la cantidad
 *        de memoria disponible, retorna -1.
 *
 * @param tabla_de_paginas
 * @param tamanio_nuevo
 * @return `int8_t` : 0 si se pudo ampliar, -1 en caso de error.
 */
int8_t ampliar_memoria_para_proceso(t_proceso_tabla *tabla_de_paginas, u_int32_t tamanio_nuevo);

/**
 * @brief Reduce memoria para un proceso.
 *
 * @param tabla_de_paginas
 * @param tamanio_nuevo
 * @return `int8_t` : 0 si se pudo reducir, -1 en caso de error.
 */
int8_t reducir_memoria_para_proceso(t_proceso_tabla *tabla_de_paginas, u_int32_t tamanio_nuevo);

void escribir_memoria_usuario(u_int32_t pid, t_list *direcciones_fisicas, void *buffer, u_int32_t tamanio, int32_t fd);

void leer_memoria_usuario(u_int32_t pid, t_list *direcciones_fisicas, u_int32_t tamanio, int32_t fd);

/**
 * @brief Función destructor de la memoria de usuario.
 */
void destruir_memoria_usuario();

#endif // MEM_USUARIO_H