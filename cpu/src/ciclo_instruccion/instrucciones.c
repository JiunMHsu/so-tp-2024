
#include "instrucciones.h"

t_dictionary *instrucciones;
t_dictionary *registros;
extern t_pcb *pcb;

void inicializar_instrucciones(void)
{
   instrucciones = dictionary_create();
   registros = dictionary_create();

   dictionary_put(instrucciones, "SET", &set);
   dictionary_put(instrucciones, "SUM", &sum);
   dictionary_put(instrucciones, "SUB", &sub);
   dictionary_put(instrucciones, "JNZ", &jnz);
   dictionary_put(instrucciones, "RESIZE", &resize);
   dictionary_put(instrucciones, "MOV_IN", &mov_in);
   dictionary_put(instrucciones, "MOV_OUT", &mov_out);
   dictionary_put(instrucciones, "COPY_STRING", &copy_string);
   dictionary_put(instrucciones, "IO_GEN_SLEEP", &io_gen_sleep);
   dictionary_put(instrucciones, "IO_STDIN_READ", &io_stdin_read);
   dictionary_put(instrucciones, "IO_STDOUT_WRITE", &io_stdout_write);
   dictionary_put(instrucciones, "EXIT", &exit_instruction);
}

void (*get_instruccion(char *instruccion))(char **)
{
   return dictionary_get(instrucciones, instruccion);
}

void set(char **parametros)
{
   int valor = atoi(parametros[1]);
   if (string_starts_with(parametros[0], "E"))
   {
      u_int32_t *registro = dictionary_get(registros, parametros[0]);
      *registro = valor;
   }
   else
   {
      u_int8_t *registro = dictionary_get(registros, parametros[0]);
      *registro = valor;
   }
}

void mov_in(char **parametros_recibidos) //  MOV_IN (Registro Datos, Registro Dirección)
{
   elementos elementos = obtenerElementos(parametros_recibidos, 1);

   u_int32_t tamanio_registro = obtener_tamanio_registro(parametros_recibidos[0]);

   t_cpu_mem_req *mem_request;
   parametros parametros_leer;

   parametros_leer = crearParametrosLeer(elementos.direcciones_fisicas, tamanio_registro);
   mem_request = crear_cpu_mem_request(LEER, pcb->pid, parametros_leer);
   enviar_mem_request(mem_request);

   t_mem_buffer_response *response = recibir_buffer_response_de_memoria();
   if (response->tamanio_buffer == tamanio_registro)
   {
      char *direccion_fisica = (string_split(elementos.direcciones_fisicas, " "))[0];
      if (tamanio_registro == 1)
      {
         *(elementos.registro_datos.registro_datos_8) = *(u_int8_t *)response->buffer;
         log_escritura_lectura_memoria(pcb->pid, READ, atoi(direccion_fisica), string_itoa(*elementos.registro_datos.registro_datos_8));
      }
      else
      {
         *(elementos.registro_datos.registro_datos_32) = *(u_int32_t *)(response->buffer);
         log_escritura_lectura_memoria(pcb->pid, READ, atoi(direccion_fisica), string_itoa(*elementos.registro_datos.registro_datos_32));
      }
   }
   else
      perror("Error al leer en memoria\n");

   destruir_buffer_response(response);
}

void mov_out(char **parametros_recibidos) //  MOV_OUT (Registro Dirección, Registro Datos)
{

   elementos elementos = obtenerElementos(parametros_recibidos, 0);
   u_int32_t tamanio_registro = obtener_tamanio_registro(parametros_recibidos[1]);

   t_cpu_mem_req *mem_request;
   parametros parametros_escribir;

   void *buffer = malloc(tamanio_registro); // ESTO NO VA, REPENSAR
   if (tamanio_registro == 1)
      memcpy(buffer, elementos.registro_datos.registro_datos_8, tamanio_registro);
   else
      memcpy(buffer, elementos.registro_datos.registro_datos_32, tamanio_registro);

   parametros_escribir = crearParametrosEscribir(elementos.direcciones_fisicas, buffer, tamanio_registro);
   mem_request = crear_cpu_mem_request(ESCRIBIR, pcb->pid, parametros_escribir);

   enviar_mem_request(mem_request);

   if (recibir_response_de_memoria() == OPERATION_SUCCEED)
   {
      char *direccion_fisica = (string_split(elementos.direcciones_fisicas, " "))[0];
      if (tamanio_registro == 1)
         log_escritura_lectura_memoria(pcb->pid, WRITE, atoi(direccion_fisica), string_itoa(*elementos.registro_datos.registro_datos_8));
      else
         log_escritura_lectura_memoria(pcb->pid, WRITE, atoi(direccion_fisica), string_itoa(*elementos.registro_datos.registro_datos_32));
   }
   else
      perror("Error al escribir en memoria\n");
}

void sum(char **parametros)
{
   void *registro1 = dictionary_get(registros, parametros[0]);
   void *registro2 = dictionary_get(registros, parametros[1]);

   if (string_starts_with(parametros[0], "E"))
      *(u_int32_t *)registro1 += *(u_int32_t *)registro2;
   else
      *(u_int8_t *)registro1 += *(u_int8_t *)registro2;
}

void sub(char **parametros)
{
   void *registro1 = dictionary_get(registros, parametros[0]);
   void *registro2 = dictionary_get(registros, parametros[1]);

   if (string_starts_with(parametros[0], "E"))
      *(u_int32_t *)registro1 -= *(u_int32_t *)registro2;
   else
      *(u_int8_t *)registro1 -= *(u_int8_t *)registro2;
}

void jnz(char **parametros)
{
   if (dictionary_get(registros, parametros[0]) != 0)
   {
      int valor = atoi(parametros[1]);
      pcb->program_counter = valor;
   }
}

void resize(char **parametros_char)
{
   parametros parametro;
   parametro.tamanio_nuevo = atoi(parametros_char[0]);
   t_cpu_mem_req *mem_request = crear_cpu_mem_request(RESIZE, pcb->pid, parametro);
   enviar_mem_request(mem_request);

   if (recibir_response_de_memoria() == OPERATION_FAILED)
   {
      set_motivo_desalojo(pcb, ERROR);
      set_motivo_finalizacion(pcb, OUT_OF_MEMORY);
   }
}

void copy_string(char **param)
{
   int tamanio_valor = atoi(param[0]);

   char *direcciones_fisicas_SI = obtener_direcciones_fisicas(pcb->cpu_registers.SI, tamanio_valor);
   char *direccion_fisica_SI_inicial = string_split(direcciones_fisicas_SI, " ")[0];

   parametros parametros_leer = crearParametrosLeer(direcciones_fisicas_SI, tamanio_valor);
   t_cpu_mem_req *mem_request_leer = crear_cpu_mem_request(LEER, pcb->pid, parametros_leer);

   enviar_mem_request(mem_request_leer);

   char *string_escribir;
   t_mem_buffer_response *response = recibir_buffer_response_de_memoria();

   if (response->tamanio_buffer == tamanio_valor)
      string_escribir = response->buffer;
   else
      perror("Error al leer en memoria\n");

   log_escritura_lectura_memoria(pcb->pid, READ, atoi(direccion_fisica_SI_inicial), string_escribir);

   char *direcciones_fisicas_DI = obtener_direcciones_fisicas(pcb->cpu_registers.DI, tamanio_valor);
   char *direccion_fisica_DI_inicial = string_split(direcciones_fisicas_DI, " ")[0];

   parametros parametros_escribir = crearParametrosEscribir(direcciones_fisicas_DI, string_escribir, tamanio_valor);

   t_cpu_mem_req *mem_request_escribir = crear_cpu_mem_request(ESCRIBIR, pcb->pid, parametros_escribir);

   enviar_mem_request(mem_request_escribir);

   if (recibir_response_de_memoria() == OPERATION_SUCCEED)
      log_escritura_lectura_memoria(pcb->pid, WRITE, atoi(direccion_fisica_DI_inicial), string_escribir);
   else
      perror("Error al escribir en memoria\n");

   destruir_buffer_response(response);
}

void io_gen_sleep(char **parametros)
{
   t_io_request *io_request = crear_io_request(pcb->pid, parametros[0], "IO_GEN_SLEEP", parametros[1]);
   set_io_request(pcb, io_request);
   set_motivo_desalojo(pcb, IO);
}

void io_stdin_read(char **parametros)
{
   char *direcciones_tamanio = obtenerElem(eliminar_primer_elemento(parametros));

   t_io_request *io_request = crear_io_request(pcb->pid, parametros[0], "IO_STDIN_READ", direcciones_tamanio);
   set_io_request(pcb, io_request);
   set_motivo_desalojo(pcb, IO);
}

void io_stdout_write(char **parametros)
{
   char *direcciones_tamanio = obtenerElem(eliminar_primer_elemento(parametros));

   t_io_request *io_request = crear_io_request(pcb->pid, parametros[0], "IO_STDOUT_WRITE", direcciones_tamanio);
   set_io_request(pcb, io_request);
   set_motivo_desalojo(pcb, IO);
}

void exit_instruction(char **parametros)
{
   set_motivo_desalojo(pcb, TERMINATED);
}

void set_registros()
{
   dictionary_put(registros, "AX", &(pcb->cpu_registers.AX));
   dictionary_put(registros, "BX", &(pcb->cpu_registers.BX));
   dictionary_put(registros, "CX", &(pcb->cpu_registers.CX));
   dictionary_put(registros, "DX", &(pcb->cpu_registers.DX));
   dictionary_put(registros, "EAX", &(pcb->cpu_registers.EAX));
   dictionary_put(registros, "EBX", &(pcb->cpu_registers.EBX));
   dictionary_put(registros, "ECX", &(pcb->cpu_registers.ECX));
   dictionary_put(registros, "EDX", &(pcb->cpu_registers.EDX));
   dictionary_put(registros, "SI", &(pcb->cpu_registers.SI));
   dictionary_put(registros, "DI", &(pcb->cpu_registers.DI));
   dictionary_put(registros, "PC", &(pcb->program_counter));
}

char *obtener_direcciones_fisicas(u_int32_t direccion_logica, u_int32_t tamanio_registro)
{
   int32_t tamanio_pagina = get_tamanio_pagina();
   int32_t pagina_inicial = direccion_logica / tamanio_pagina;
   int32_t tamanio_registro_aux = tamanio_registro;
   int32_t direccion_logica_aux = (int32_t)direccion_logica;

   int32_t pagina_final = (direccion_logica_aux + tamanio_registro_aux - 1) / tamanio_pagina;

   pagina_final = pagina_final < 0 ? 0 : pagina_final;

   char *direcciones_fisicas = string_new();
   char *direccion_fisica_actual_str = string_itoa(get_direccion_fisica(pcb->pid, direccion_logica));
   string_append(&direcciones_fisicas, direccion_fisica_actual_str);

   for (u_int32_t pagina = pagina_inicial; pagina < pagina_final; pagina++) // Recorre las páginas necesarias para leer el registro
   {
      u_int32_t pagina_siguiente = pagina + 1;
      direccion_fisica_actual_str = string_itoa(get_direccion_fisica(pcb->pid, pagina_siguiente * tamanio_pagina)); // Esto debería devolver la dirección física de la página nueva que se necesita
      string_append(&direcciones_fisicas, " ");
      string_append(&direcciones_fisicas, direccion_fisica_actual_str);
   }

   return direcciones_fisicas;
}

parametros crearParametrosLeer(char *direccion_fisica, u_int32_t tamanio_valor)
{
   parametros parametros_leer;
   parametros_leer.param_leer.direcciones_fisicas = direccion_fisica;
   parametros_leer.param_leer.tamanio_buffer = tamanio_valor;
   return parametros_leer;
}

parametros crearParametrosEscribir(char *direccion_fisica, void *buffer, u_int32_t tamanio_valor)
{
   parametros parametros_escribir;
   parametros_escribir.param_escribir.direcciones_fisicas = direccion_fisica;
   parametros_escribir.param_escribir.tamanio_buffer = tamanio_valor;
   parametros_escribir.param_escribir.buffer = buffer;
   return parametros_escribir;
}

u_int32_t obtener_tamanio_registro(char *parametros_recibidos)
{
   if (string_starts_with(parametros_recibidos, "E"))
   {
      return 4;
   }
   else
   {
      return 1;
   }
}

elementos obtenerElementos(char **parametros_recibidos, int num)
{
   elementos elementos;

   if (obtener_tamanio_registro(parametros_recibidos[0]) == 1 && obtener_tamanio_registro(parametros_recibidos[1]) == 1)
   {
      if (num == 0) // direccion - dato
      {
         elementos.direccion_logica.direccion_logica_8 = dictionary_get(registros, parametros_recibidos[0]);
         elementos.registro_datos.registro_datos_8 = dictionary_get(registros, parametros_recibidos[1]);
         elementos.direcciones_fisicas = obtener_direcciones_fisicas(*(elementos.direccion_logica.direccion_logica_8), sizeof(*(elementos.registro_datos.registro_datos_8)));
      }
      else // dato - direccion
      {
         elementos.registro_datos.registro_datos_8 = dictionary_get(registros, parametros_recibidos[0]);
         elementos.direccion_logica.direccion_logica_8 = dictionary_get(registros, parametros_recibidos[1]);
         elementos.direcciones_fisicas = obtener_direcciones_fisicas(*(elementos.direccion_logica.direccion_logica_8), sizeof(*(elementos.registro_datos.registro_datos_8)));
      }
   }
   else if (obtener_tamanio_registro(parametros_recibidos[0]) == 1 && obtener_tamanio_registro(parametros_recibidos[1]) == 4)
   {
      if (num == 0) // direccion - dato
      {
         elementos.direccion_logica.direccion_logica_8 = dictionary_get(registros, parametros_recibidos[0]);
         elementos.registro_datos.registro_datos_32 = dictionary_get(registros, parametros_recibidos[1]);
         elementos.direcciones_fisicas = obtener_direcciones_fisicas(*(elementos.direccion_logica.direccion_logica_8), sizeof(*(elementos.registro_datos.registro_datos_32)));
      }
      else // dato - direccion
      {
         elementos.registro_datos.registro_datos_8 = dictionary_get(registros, parametros_recibidos[0]);
         elementos.direccion_logica.direccion_logica_32 = dictionary_get(registros, parametros_recibidos[1]);
         elementos.direcciones_fisicas = obtener_direcciones_fisicas(*(elementos.direccion_logica.direccion_logica_32), sizeof(*(elementos.registro_datos.registro_datos_8)));
      }
   }
   else if (obtener_tamanio_registro(parametros_recibidos[0]) == 4 && obtener_tamanio_registro(parametros_recibidos[1]) == 1)
   {
      if (num == 0) // direccion - dato
      {
         elementos.direccion_logica.direccion_logica_32 = dictionary_get(registros, parametros_recibidos[0]);
         elementos.registro_datos.registro_datos_8 = dictionary_get(registros, parametros_recibidos[1]);
         elementos.direcciones_fisicas = obtener_direcciones_fisicas(*(elementos.direccion_logica.direccion_logica_32), sizeof(*(elementos.registro_datos.registro_datos_8)));
      }
      else // dato - direccion
      {
         elementos.registro_datos.registro_datos_32 = dictionary_get(registros, parametros_recibidos[0]);
         elementos.direccion_logica.direccion_logica_8 = dictionary_get(registros, parametros_recibidos[1]);
         elementos.direcciones_fisicas = obtener_direcciones_fisicas(*(elementos.direccion_logica.direccion_logica_8), sizeof(*(elementos.registro_datos.registro_datos_32)));
      }
   }
   else
   {
      if (num == 0) // direccion - dato
      {
         elementos.direccion_logica.direccion_logica_32 = dictionary_get(registros, parametros_recibidos[0]);
         elementos.registro_datos.registro_datos_32 = dictionary_get(registros, parametros_recibidos[1]);
         elementos.direcciones_fisicas = obtener_direcciones_fisicas(*(elementos.direccion_logica.direccion_logica_32), sizeof(*(elementos.registro_datos.registro_datos_32)));
      }
      else // dato - direccion
      {
         elementos.registro_datos.registro_datos_32 = dictionary_get(registros, parametros_recibidos[0]);
         elementos.direccion_logica.direccion_logica_32 = dictionary_get(registros, parametros_recibidos[1]);
         elementos.direcciones_fisicas = obtener_direcciones_fisicas(*(elementos.direccion_logica.direccion_logica_32), sizeof(*(elementos.registro_datos.registro_datos_32)));
      }
   }

   return elementos;
}

// siempre direccion - dato
char *obtenerElem(char **parametros_recibidos)
{
   elementos elementos;

   if (obtener_tamanio_registro(parametros_recibidos[0]) == 1 && obtener_tamanio_registro(parametros_recibidos[1]) == 1)
   {
      elementos.direccion_logica.direccion_logica_8 = dictionary_get(registros, parametros_recibidos[0]);
      elementos.registro_datos.registro_datos_8 = dictionary_get(registros, parametros_recibidos[1]);
      elementos.direcciones_fisicas = obtener_direcciones_fisicas(*(elementos.direccion_logica.direccion_logica_8), *(elementos.registro_datos.registro_datos_8));
   }
   else if (obtener_tamanio_registro(parametros_recibidos[0]) == 1 && obtener_tamanio_registro(parametros_recibidos[1]) == 4)
   {
      elementos.direccion_logica.direccion_logica_8 = dictionary_get(registros, parametros_recibidos[0]);
      elementos.registro_datos.registro_datos_32 = dictionary_get(registros, parametros_recibidos[1]);
      elementos.direcciones_fisicas = obtener_direcciones_fisicas(*(elementos.direccion_logica.direccion_logica_8), *(elementos.registro_datos.registro_datos_32));
   }
   else if (obtener_tamanio_registro(parametros_recibidos[0]) == 4 && obtener_tamanio_registro(parametros_recibidos[1]) == 1)
   {
      elementos.direccion_logica.direccion_logica_32 = dictionary_get(registros, parametros_recibidos[0]);
      elementos.registro_datos.registro_datos_8 = dictionary_get(registros, parametros_recibidos[1]);
      elementos.direcciones_fisicas = obtener_direcciones_fisicas(*(elementos.direccion_logica.direccion_logica_32), *(elementos.registro_datos.registro_datos_8));
   }
   else
   {
      elementos.direccion_logica.direccion_logica_32 = dictionary_get(registros, parametros_recibidos[0]);
      elementos.registro_datos.registro_datos_32 = dictionary_get(registros, parametros_recibidos[1]);
      elementos.direcciones_fisicas = obtener_direcciones_fisicas(*(elementos.direccion_logica.direccion_logica_32), *(elementos.registro_datos.registro_datos_32));
   }

   char *tamanio_valor;

   if (obtener_tamanio_registro(parametros_recibidos[1]) == 1)
   {
      tamanio_valor = string_itoa(*elementos.registro_datos.registro_datos_8);
   }
   else if (obtener_tamanio_registro(parametros_recibidos[1]) == 4)
   {
      tamanio_valor = string_itoa(*elementos.registro_datos.registro_datos_32);
   }
   // char *tamanio_valor = string_itoa(*elementos.registro_tamanio);

   char *direcciones_tamanio = string_new();

   // concatenar tamanio con direcciones fisicas
   string_append(&direcciones_tamanio, elementos.direcciones_fisicas);
   string_append(&direcciones_tamanio, " ");
   string_append(&direcciones_tamanio, tamanio_valor);

   printf("DIRECCIONES Y TAMANIO: %s\n", direcciones_tamanio);

   return direcciones_tamanio;
}