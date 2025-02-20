#ifndef UTILS_PROTOCOL_H
#define UTILS_PROTOCOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <commons/collections/list.h>

typedef enum
{
    NUMBER,
    MESSAGE,
    PACKET
} op_code;

typedef struct
{
    int32_t size;
    void *stream;
} t_buffer;

typedef struct
{
    op_code op_code;
    t_buffer *buffer;
} t_packet;

t_packet *crear_paquete(void);
void crear_buffer(t_packet *paquete);
void agregar_a_paquete(t_packet *paquete, void *contenido, int32_t tamanio);
void *serializar_paquete(t_packet *paquete, int32_t tamanio_paquete);
void enviar_paquete(t_packet *paquete, int32_t fd_conexion);
void eliminar_paquete(t_packet *paquete);

int32_t recibir_operacion(int32_t fd_conexion);
void *recibir_buffer(int32_t *tamanio, int32_t fd_conexion);
t_list *recibir_paquete(int32_t fd_conexion);

void enviar_senial(int32_t signal, int32_t fd_conexion);
int32_t recibir_senial(int32_t fd_conexion);

void enviar_mensaje(char *mensaje, int32_t fd_conexion);
char *recibir_mensaje(int32_t fd_conexion);

#endif // UTILS_PROTOCOL_H
