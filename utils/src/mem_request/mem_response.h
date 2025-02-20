#ifndef UTILS_MEM_REQUEST_RESPONSE_H
#define UTILS_MEM_REQUEST_RESPONSE_H

#include <protocol/protocol.h>

typedef enum
{
   OPERATION_SUCCEED,
   OPERATION_FAILED,
} t_mem_response;

typedef struct
{
   t_mem_response resultado;
   void *buffer;
   u_int32_t tamanio_buffer;
} t_mem_buffer_response;

t_mem_buffer_response *crear_buffer_response(t_mem_response resultado, void *buffer, u_int32_t tamanio_buffer);
t_packet *serializar_buffer_response(t_mem_buffer_response *buffer_response);
void enviar_buffer_response(int32_t fd_conexion, t_mem_buffer_response *buffer_response);
t_mem_buffer_response *recibir_buffer_response(int32_t fd_conexion);
void destruir_buffer_response(t_mem_buffer_response *buffer_response);

#endif // UTILS_MEM_REQUEST_RESPONSE_H