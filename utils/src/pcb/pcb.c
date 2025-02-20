#include "pcb.h"

t_pcb *crear_pcb(u_int32_t pid, char *ejecutable)
{
   t_pcb *pcb = malloc(sizeof(t_pcb));

   pcb->pid = pid;
   pcb->program_counter = 0;
   pcb->quantum = 0;
   pcb->priority = 0;
   pcb->cpu_registers = crear_registros();
   pcb->executable = strdup(ejecutable);
   pcb->resource = strdup("");
   pcb->io_request = crear_io_request(pcb->pid, "", "", "");
   pcb->motivo_desalojo = NONE;
   pcb->motivo_finalizacion = -1;
   pcb->estado = -1;

   return pcb;
}

void destruir_pcb(t_pcb *pcb)
{
   free(pcb->executable);
   free(pcb->resource);
   destruir_io_request(pcb->io_request);
   free(pcb);
}

t_packet *serializar_pcb(t_pcb *pcb)
{
   t_packet *paquete = crear_paquete();

   agregar_a_paquete(paquete, &(pcb->pid), sizeof(u_int32_t));
   agregar_a_paquete(paquete, &(pcb->program_counter), sizeof(int32_t));
   agregar_a_paquete(paquete, &(pcb->quantum), sizeof(u_int32_t));
   agregar_a_paquete(paquete, &(pcb->priority), sizeof(int8_t));

   // serializar registros
   agregar_a_paquete(paquete, &(pcb->cpu_registers.AX), sizeof(u_int8_t));
   agregar_a_paquete(paquete, &(pcb->cpu_registers.BX), sizeof(u_int8_t));
   agregar_a_paquete(paquete, &(pcb->cpu_registers.CX), sizeof(u_int8_t));
   agregar_a_paquete(paquete, &(pcb->cpu_registers.DX), sizeof(u_int8_t));
   agregar_a_paquete(paquete, &(pcb->cpu_registers.EAX), sizeof(u_int32_t));
   agregar_a_paquete(paquete, &(pcb->cpu_registers.EBX), sizeof(u_int32_t));
   agregar_a_paquete(paquete, &(pcb->cpu_registers.ECX), sizeof(u_int32_t));
   agregar_a_paquete(paquete, &(pcb->cpu_registers.EDX), sizeof(u_int32_t));
   agregar_a_paquete(paquete, &(pcb->cpu_registers.SI), sizeof(u_int32_t));
   agregar_a_paquete(paquete, &(pcb->cpu_registers.DI), sizeof(u_int32_t));

   agregar_a_paquete(paquete, pcb->executable, strlen(pcb->executable) + 1);
   agregar_a_paquete(paquete, pcb->resource, strlen(pcb->resource) + 1);

   // serializar io_request
   agregar_a_paquete(paquete, pcb->io_request->interface_name, strlen(pcb->io_request->interface_name) + 1);
   agregar_a_paquete(paquete, pcb->io_request->instruction, strlen(pcb->io_request->instruction) + 1);
   agregar_a_paquete(paquete, pcb->io_request->arguments, strlen(pcb->io_request->arguments) + 1);

   agregar_a_paquete(paquete, &(pcb->motivo_desalojo), sizeof(motivo_desalojo));
   agregar_a_paquete(paquete, &(pcb->motivo_finalizacion), sizeof(motivo_finalizacion));
   agregar_a_paquete(paquete, &(pcb->estado), sizeof(state));

   return paquete;
}

void enviar_pcb(int32_t fd_conexion, t_pcb *pcb)
{
   t_packet *paquete = serializar_pcb(pcb);
   enviar_paquete(paquete, fd_conexion);
   eliminar_paquete(paquete);
}

t_pcb *recibir_pcb(int32_t fd_conexion)
{
   t_list *paquete = recibir_paquete(fd_conexion);
   if (paquete == NULL)
      return NULL;

   t_pcb *pcb = malloc(sizeof(t_pcb));

   pcb->pid = *(u_int32_t *)list_get(paquete, 0);
   pcb->program_counter = *(int32_t *)list_get(paquete, 1);
   pcb->quantum = *(u_int32_t *)list_get(paquete, 2);
   pcb->priority = *(int8_t *)list_get(paquete, 3);

   pcb->cpu_registers.AX = *(u_int8_t *)list_get(paquete, 4);
   pcb->cpu_registers.BX = *(u_int8_t *)list_get(paquete, 5);
   pcb->cpu_registers.CX = *(u_int8_t *)list_get(paquete, 6);
   pcb->cpu_registers.DX = *(u_int8_t *)list_get(paquete, 7);
   pcb->cpu_registers.EAX = *(u_int32_t *)list_get(paquete, 8);
   pcb->cpu_registers.EBX = *(u_int32_t *)list_get(paquete, 9);
   pcb->cpu_registers.ECX = *(u_int32_t *)list_get(paquete, 10);
   pcb->cpu_registers.EDX = *(u_int32_t *)list_get(paquete, 11);
   pcb->cpu_registers.SI = *(u_int32_t *)list_get(paquete, 12);
   pcb->cpu_registers.DI = *(u_int32_t *)list_get(paquete, 13);

   pcb->executable = strdup((char *)list_get(paquete, 14));
   pcb->resource = strdup((char *)list_get(paquete, 15));

   pcb->io_request = crear_io_request(pcb->pid,
                                      (char *)list_get(paquete, 16),
                                      (char *)list_get(paquete, 17),
                                      (char *)list_get(paquete, 18));

   pcb->motivo_desalojo = *(motivo_desalojo *)list_get(paquete, 19);
   pcb->motivo_finalizacion = *(motivo_finalizacion *)list_get(paquete, 20);
   pcb->estado = *(state *)list_get(paquete, 21);

   list_destroy_and_destroy_elements(paquete, &free);
   return pcb;
}

void actualizar_pcb(t_pcb *pcb, t_pcb *nuevo_pcb)
{
   pcb->program_counter = nuevo_pcb->program_counter;
   pcb->quantum = nuevo_pcb->quantum;
   pcb->cpu_registers = nuevo_pcb->cpu_registers;

   set_recurso_pcb(pcb, nuevo_pcb->resource);
   set_io_request(pcb, nuevo_pcb->io_request);

   pcb->motivo_desalojo = nuevo_pcb->motivo_desalojo;
   pcb->motivo_finalizacion = nuevo_pcb->motivo_finalizacion;
}

void set_quantum_pcb(t_pcb *pcb, u_int32_t quantum)
{
   pcb->quantum = quantum;
}

void set_recurso_pcb(t_pcb *pcb, char *recurso)
{
   free(pcb->resource);
   pcb->resource = strdup(recurso);
}

void set_estado_pcb(t_pcb *pcb, state estado)
{
   pcb->estado = estado;
}

void set_motivo_desalojo(t_pcb *pcb, motivo_desalojo motivo)
{
   pcb->motivo_desalojo = motivo;
}

void set_motivo_finalizacion(t_pcb *pcb, motivo_finalizacion motivo)
{
   pcb->motivo_finalizacion = motivo;
}

void set_io_request(t_pcb *pcb, t_io_request *io_request)
{
   destruir_io_request(pcb->io_request);
   pcb->io_request = crear_io_request(io_request->pid,
                                      io_request->interface_name,
                                      io_request->instruction,
                                      io_request->arguments);
}

void set_prioridad(t_pcb *pcb, int8_t priority)
{
   pcb->priority = priority;
}

void debug_pcb(t_pcb *pcb)
{
   printf("PID: %d\n", pcb->pid);
   printf("Program Counter: %d\n", pcb->program_counter);
   printf("Quantum: %d\n", pcb->quantum);
   printf("Prioridad: %d\n", pcb->priority);

   printf("AX: %d\n", pcb->cpu_registers.AX);
   printf("BX: %d\n", pcb->cpu_registers.BX);
   printf("CX: %d\n", pcb->cpu_registers.CX);
   printf("DX: %d\n", pcb->cpu_registers.DX);
   printf("EAX: %d\n", pcb->cpu_registers.EAX);
   printf("EBX: %d\n", pcb->cpu_registers.EBX);
   printf("ECX: %d\n", pcb->cpu_registers.ECX);
   printf("EDX: %d\n", pcb->cpu_registers.EDX);
   printf("ECX: %d\n", pcb->cpu_registers.SI);
   printf("EDX: %d\n", pcb->cpu_registers.DI);

   printf("Recurso: %s\n", pcb->resource);
   printf("Executable Path: %s\n", pcb->executable);
   printf("Motivo Desalojo: %d\n", pcb->motivo_desalojo);
   printf("Motivo Finalizacion: %d\n", pcb->motivo_finalizacion);
   printf("Estado: %d\n", pcb->estado);

   if (pcb->io_request != NULL)
   {
      print_io_request(pcb->io_request);
   }
}
