#ifndef UTILS_MEM_REQUEST_KERNEL_H
#define UTILS_MEM_REQUEST_KERNEL_H

#include <protocol/protocol.h>

#include "mem_response.h"

typedef enum
{
   INICIAR_PROCESO,
   FINALIZAR_PROCESO,
} kernel_req_operation;

typedef struct
{
   kernel_req_operation operacion;
   u_int32_t pid; // El PID está presente en todos los casos
   char *path;    // Presente solo para INICIAR_PROCESO
} t_kernel_mem_req;

t_kernel_mem_req *crear_kernel_mem_request(kernel_req_operation operacion, u_int32_t pid, char *path);
void enviar_kernel_mem_request(int32_t fd_memoria, t_kernel_mem_req *mem_request);
t_kernel_mem_req *recibir_kernel_mem_request(int32_t fd_kernel);
void destruir_kernel_mem_request(t_kernel_mem_req *mem_request);

#endif // UTILS_MEM_REQUEST_KERNEL_H