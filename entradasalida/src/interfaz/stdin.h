#ifndef STDIN_H
#define STDIN_H

#include <stdlib.h>
#include <stdio.h>
#include <commons/string.h>
#include <string.h>
#include <sockets/sockets.h>

#include "config/config.h"
#include "logger/logger.h"
#include "conexion/kernel.h"


void iniciar_rutina_interfaz_stdin(void);
void io_stdin_read(char *);

#endif // STDIN_H