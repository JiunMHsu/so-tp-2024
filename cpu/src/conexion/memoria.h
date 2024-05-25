#ifndef CONEXION_MEMORIA_H
#define CONEXION_MEMORIA_H

#include <stdlib.h>
#include <sockets/sockets.h>
#include <protocol/pcb.h>
#include <protocol/protocol.h>
#include <unistd.h>
#include <sys/socket.h>

#include "config/config.h"

int8_t conectar_con_memoria();
void hablar_con_memoria(int32_t fd_memoria);
void liberar_conexion_memoria();
void enviar_pcb_memoria(t_pcb *pcb);

#endif // CONEXION_MEMORIA_H