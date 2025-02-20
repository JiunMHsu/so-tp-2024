#ifndef UTILS_IO_REQUEST_H
#define UTILS_IO_REQUEST_H

#include <protocol/protocol.h>

typedef struct
{
   u_int32_t pid;
   char *interface_name;
   char *instruction;
   char *arguments; // esta pensado como un solo string con los argumentos separados por espacios (si es que hay más de uno)
} t_io_request;

typedef enum
{
   INVALID_INSTRUCTION,
   EXECUTED,
   FAILED,
} t_io_response;

t_io_request *crear_io_request(u_int32_t pid, char *nombre_interfaz, char *instruccion, char *argumentos);
t_packet *serializar_io_request(t_io_request *io_request);
void vaciar_io_request(t_io_request *io_request);
void print_io_request(t_io_request *io_request);
void destruir_io_request(t_io_request *io_request);

void enviar_io_request(int32_t fd_conexion, t_io_request *io_request);
t_io_request *recibir_io_request(int32_t fd_conexion);

void enviar_io_response(int32_t fd_conexion, t_io_response response);
t_io_response recibir_io_response(int32_t fd_conexion);

#endif // UTILS_IO_REQUEST_H