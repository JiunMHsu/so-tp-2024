#include "protocol.h"

t_packet *crear_paquete(void)
{
   t_packet *paquete = malloc(sizeof(t_packet));
   paquete->op_code = PACKET;
   crear_buffer(paquete);
   return paquete;
}

void crear_buffer(t_packet *paquete)
{
   paquete->buffer = malloc(sizeof(t_buffer));
   paquete->buffer->size = 0;
   paquete->buffer->stream = NULL;
}

void agregar_a_paquete(t_packet *paquete, void *contenido, int32_t tamanio)
{
   // aumenta el tamaño del buffer agregando:
   // - el tamaño del contenido
   // - el tamaño de un entero que va a guardar el tamaño del contenido
   int32_t new_size = paquete->buffer->size + tamanio + sizeof(int32_t);
   paquete->buffer->stream = realloc(paquete->buffer->stream, new_size);

   int32_t offset = 0;

   // agrega el tamaño del contenido
   offset = paquete->buffer->size;
   memcpy(paquete->buffer->stream + offset, &tamanio, sizeof(int32_t));

   // agrega el contenido
   offset = paquete->buffer->size + sizeof(int32_t);
   memcpy(paquete->buffer->stream + offset, contenido, tamanio);

   // actualiza el tamaño del buffer
   paquete->buffer->size += tamanio + sizeof(int32_t);
}

void *serializar_paquete(t_packet *paquete, int32_t tamanio_paquete)
{
   void *serializado = malloc(tamanio_paquete);
   int32_t offset = 0;

   // agrega el código de operación
   memcpy(serializado + offset, &(paquete->op_code), sizeof(int32_t));
   offset += sizeof(int32_t);

   // agrega el tamaño del buffer
   memcpy(serializado + offset, &(paquete->buffer->size), sizeof(int32_t));
   offset += sizeof(int32_t);

   // agrega el contenido del buffer
   memcpy(serializado + offset, paquete->buffer->stream, paquete->buffer->size);
   // desplazamiento += paquete->buffer->size;

   return serializado;
}

void enviar_paquete(t_packet *paquete, int32_t fd_conexion)
{
   // los dos primeros int32_t son el código de operación y el tamaño del buffer
   int32_t tamanio_paquete = paquete->buffer->size + 2 * sizeof(int32_t);
   void *serializado = serializar_paquete(paquete, tamanio_paquete);

   send(fd_conexion, serializado, tamanio_paquete, 0);

   free(serializado);
}

void eliminar_paquete(t_packet *paquete)
{
   free(paquete->buffer->stream);
   free(paquete->buffer);
   free(paquete);
}

int32_t recibir_operacion(int32_t fd_conexion)
{
   op_code code;
   if (recv(fd_conexion, &code, sizeof(int32_t), MSG_WAITALL) > 0)
      return code;
   else
   {
      close(fd_conexion);
      return -1;
   }
}

void *recibir_buffer(int32_t *tamanio, int32_t fd_conexion)
{
   void *buffer;

   recv(fd_conexion, tamanio, sizeof(int32_t), MSG_WAITALL);
   buffer = malloc(*tamanio);
   recv(fd_conexion, buffer, *tamanio, MSG_WAITALL);

   return buffer;
}

t_list *recibir_paquete(int32_t fd_conexion)
{
   int32_t buffer_size = 0;
   void *buffer;
   int32_t offset = 0;
   t_list *valores = list_create();
   int32_t tamanio;

   if (recibir_operacion(fd_conexion) != PACKET)
   {
      list_destroy(valores);
      return NULL;
   }

   buffer = recibir_buffer(&buffer_size, fd_conexion);

   while (offset < buffer_size)
   {
      memcpy(&tamanio, buffer + offset, sizeof(int32_t));
      offset += sizeof(int32_t);
      void *valor = malloc(tamanio);
      memcpy(valor, buffer + offset, tamanio);
      offset += tamanio;
      list_add(valores, valor);
   }

   free(buffer);
   return valores;
}

void enviar_senial(int32_t signal, int32_t fd_conexion)
{
   op_code code = NUMBER;
   void *buffer = malloc(sizeof(int32_t) * 2);

   memcpy(buffer, &code, sizeof(int32_t));
   memcpy(buffer + sizeof(int32_t), &signal, sizeof(int32_t));

   send(fd_conexion, buffer, sizeof(int32_t) * 2, 0);
   free(buffer);
}

int32_t recibir_senial(int32_t fd_conexion)
{
   if (recibir_operacion(fd_conexion) != NUMBER)
      return -1;

   int32_t signal;
   recv(fd_conexion, &signal, sizeof(int32_t), MSG_WAITALL);
   return signal;
}

void enviar_mensaje(char *mensaje, int32_t fd_conexion)
{
   op_code code = MESSAGE;
   int32_t tamanio_mensaje = strlen(mensaje) + 1;
   int32_t buffer_size = sizeof(int32_t) * 2 + tamanio_mensaje;
   void *buffer = malloc(buffer_size);

   int32_t offset = 0;
   memcpy(buffer, &code, sizeof(int32_t));
   offset += sizeof(int32_t);
   memcpy(buffer + offset, &tamanio_mensaje, sizeof(int32_t));
   offset += sizeof(int32_t);
   memcpy(buffer + offset, mensaje, tamanio_mensaje);

   send(fd_conexion, buffer, buffer_size, 0);
   free(buffer);
}

char *recibir_mensaje(int32_t fd_conexion)
{
   if (recibir_operacion(fd_conexion) != MESSAGE)
      return NULL;

   int32_t tamanio_mensaje;
   recv(fd_conexion, &tamanio_mensaje, sizeof(int32_t), MSG_WAITALL);

   char *mensaje = malloc(tamanio_mensaje);
   recv(fd_conexion, mensaje, tamanio_mensaje, MSG_WAITALL);

   return mensaje;
}
