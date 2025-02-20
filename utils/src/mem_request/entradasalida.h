#ifndef UTILS_MEM_REQUEST_ENTRADASALIDA_H
#define UTILS_MEM_REQUEST_ENTRADASALIDA_H

#include <protocol/protocol.h>
#include <extended/string.h>

#include "mem_response.h"

typedef enum
{
   LEER_IO,
   ESCRIBIR_IO,
} io_req_operation;

typedef struct
{
   io_req_operation operacion;
   u_int32_t pid;
   char *direcciones_fisicas;
   u_int32_t tamanio_buffer;
   void *buffer;
} t_io_mem_req;

t_io_mem_req *crear_io_mem_request(io_req_operation operacion, u_int32_t pid, char *direcciones_fisicas, u_int32_t tamanio_buffer, void *buffer);
void enviar_io_mem_request(int32_t fd_memoria, t_io_mem_req *mem_request);
t_io_mem_req *recibir_io_mem_request(int32_t fd);
void destruir_io_mem_request(t_io_mem_req *mem_request);

#endif // UTILS_MEM_REQUEST_ENTRADASALIDA_H