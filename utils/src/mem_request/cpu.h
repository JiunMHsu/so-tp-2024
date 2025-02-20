#ifndef UTILS_MEM_REQUEST_CPU_H
#define UTILS_MEM_REQUEST_CPU_H

#include <protocol/protocol.h>
#include <extended/string.h>

#include "mem_response.h"

typedef enum
{
   FETCH_INSTRUCCION,
   OBTENER_MARCO,
   LEER,
   ESCRIBIR,
   RESIZE
} cpu_req_operation;

typedef struct
{
   cpu_req_operation operacion;
   u_int32_t pid;
   u_int32_t program_counter; // Presente solo para FETCH_INSTRUCCION
   u_int32_t nro_pag;         // Presente solo para OBTENER_MARCO
   u_int32_t tamanio_nuevo;   // Presente solo para RESIZE
   char *direcciones_fisicas; // Presente para LEER y ESCRIBIR
   u_int32_t tamanio_buffer;  // Presente para LEER y ESCRIBIR
   void *buffer;              // Presente solo para ESCRIBIR
} t_cpu_mem_req;

t_cpu_mem_req *crear_instruccion_request(u_int32_t pid, u_int32_t program_counter);
t_cpu_mem_req *crear_nro_frame_request(u_int32_t pid, u_int32_t nro_pag);
t_cpu_mem_req *crear_resize_request(u_int32_t pid, u_int32_t tamanio_nuevo);
t_cpu_mem_req *crear_cpu_read_request(u_int32_t pid, char *direcciones_fisicas, u_int32_t tamanio_buffer);
t_cpu_mem_req *crear_cpu_write_request(u_int32_t pid, char *direcciones_fisicas, u_int32_t tamanio_buffer, void *buffer);

void enviar_cpu_mem_request(int32_t fd_memoria, t_cpu_mem_req *mem_request);
t_cpu_mem_req *recibir_cpu_mem_request(int32_t fd_cpu);
void destruir_cpu_mem_request(t_cpu_mem_req *mem_request);

t_list *convertir_a_lista_de_direcciones_fisicas(char *direcciones_fisicas);

#endif // UTILS_MEM_REQUEST_CPU_H