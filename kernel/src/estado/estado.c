#include "estado.h"

q_estado *crear_estado(state codigo_estado)
{
   q_estado *estado = malloc(sizeof(q_estado));

   estado->lista = mlist_create();
   estado->hay_proceso = malloc(sizeof(sem_t));
   estado->cod_estado = codigo_estado;
   sem_init(estado->hay_proceso, 0, 0);

   return estado;
}

void push_proceso(q_estado *estado, t_pcb *pcb)
{
   mlist_push_as_queue(estado->lista, pcb);

   state anterior = pcb->estado;
   pcb->estado = estado->cod_estado;
   log_cambio_de_estado(pcb->pid, anterior, pcb->estado);

   sem_post(estado->hay_proceso);
}

void *pop_proceso(q_estado *estado)
{
   sem_wait(estado->hay_proceso);
   return mlist_pop_as_queue(estado->lista);
}

void destruir_estado(q_estado *estado)
{
   mlist_destroy(estado->lista);
   sem_destroy(estado->hay_proceso);

   free(estado);
}
