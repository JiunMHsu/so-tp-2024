#include "ciclo.h"

t_dictionary *instrucciones;
t_dictionary *registros;
t_registers registros_cpu; //Setearlo con el PCB

extern int hay_interrupcion;
extern pthread_mutex_t mutexInterrupcion;

char *fetch()
{
   enviar_pcb_memoria(pcb);
   char *instruccion = recibir_instruccion(fd_memoria); // no se por que no me lo toma
   return instruccion;
}

void (*decode(char *char_instruccion))(Parametros)
{
   char **instruc_parametros = instruccion_parametros(char_instruccion);
   // char **instruccion_parametros;
   // instruccion_parametros = string_split(char_instruccion_completa, " ");
   instrucciones = dictionary_create();
   set_diccionario_instrucciones(instrucciones); // ver porque cada vez que busque operando tiene que llenar el diciconario
   if (dictionary_has_key(instrucciones, instruc_parametros[0]))
   {
      return dictionary_get(instrucciones, instruc_parametros[0]);
   }
   else
   {
      // En caso de instrucción desconocida
      printf("Instrucción desconocida: %s\n", instruc_parametros[0]);
      return NULL;
   }
}

Parametros obtener_parametros(char **parametros) // CHEQUEAR SI ES QUE RECIBE NONE
{
   // RECIBE UN ARRAY CON LA INTRUCCION EN LA PRIMERA POSICION, POR LO QUE HAY QUE COMENZAR DESDE 1
   Parametros struct_parametros;
   struct_parametros.parametro1 = buscar_operando(parametros[1]);
   struct_parametros.parametro2 = buscar_operando(parametros[2]);
   // struct_parametros.parametro3 = buscar_operando(parametros[3]) // siguientes checkpoints
   // struct_parametros.parametro4 = buscar_operando(parametros[4])
   return struct_parametros;
}

Parametro buscar_operando(char *parametro)
{
   registros = dictionary_create();
   set_diccionario_registros(registros); // ver porque cada vez que busque operando tiene que llenar el diciconario
   Parametro operando;
   if (es_numero(parametro))
   {
      operando.tipo_dato = VALOR;
      operando.dato.valor = char_a_numero(parametro);
      return operando; // ver si usar el mas robusto
   }
   else if (dictionary_has_key(registros, parametro))
   {
      if (string_starts_with(parametro, "E"))
      {
         operando.tipo_dato = INT32;
         operando.dato.registro_u32 = dictionary_get(registros, parametro);
         return operando;
      }
      else
      {
         operando.tipo_dato = INT8;
         operando.dato.registro_u8 = dictionary_get(registros, parametro);
         return operando;
      }
   }
   else
   {
      operando.tipo_dato = INTERFAZ;
      operando.dato.interfaz = parametro;
      return operando;
   }
}

char **instruccion_parametros(char *char_instruccion)
{
   return string_split(char_instruccion, " ");
}

void execute(void (*instruccion)(Parametros), char *char_instruccion)
{
   aumentar_program_counter();
   char **instruc_parametros = instruccion_parametros(char_instruccion);
   instruccion(obtener_parametros(instruc_parametros));
}

void aumentar_program_counter() /// VER SI VA  ACA
{
   registros_cpu.PC += 1;
}

int check_interrupt()
{
   pthread_mutex_lock(&mutexInterrupcion);


   if(hay_interrupcion == 1){
      pthread_mutex_unlock(&mutexInterrupcion);
      return 1;
   }
   return 0;

}

void check_desalojo()
{
   printf("Check desalojo\n");
}

void ciclo_instruccion()
{
   while (1)
   {
      char *char_instruccion = fetch();

      void (*instruccion)(Parametros) = decode(char_instruccion);

      execute(instruccion, char_instruccion);

      check_desalojo(); // si ocurren simultaneamente pesa mas I/O

      if(check_interrupt())
      {
         //Ver que hacer aca para interrumpir
      }


   }
}

///////// ESTRUCTURAS AUXILIARES ///////////

void set_diccionario_instrucciones(t_dictionary *instrucciones)
{
   dictionary_put(instrucciones, "SET", &set);
   dictionary_put(instrucciones, "SUM", &sum);
   dictionary_put(instrucciones, "SUB", &sub);
   dictionary_put(instrucciones, "JNZ", &jnz);
   dictionary_put(instrucciones, "IO_GEN_SLEEP", &io_gen_sleep);
}

void set_diccionario_registros(t_dictionary *registros)
{
   dictionary_put(registros, "AX", &(registros_cpu.registers_generales.AX));
   dictionary_put(registros, "BX", &(registros_cpu.registers_generales.BX));
   dictionary_put(registros, "CX", &(registros_cpu.registers_generales.CX));
   dictionary_put(registros, "DX", &(registros_cpu.registers_generales.DX));
   dictionary_put(registros, "EAX", &(registros_cpu.registers_generales.EAX));
   dictionary_put(registros, "EBX", &(registros_cpu.registers_generales.EBX));
   dictionary_put(registros, "ECX", &(registros_cpu.registers_generales.ECX));
   dictionary_put(registros, "EDX", &(registros_cpu.registers_generales.EDX));
}

//////////FUNCIONES AUXILIARES////////////

int es_numero(char *parametro)
{
   while (*parametro)
   {
      if (*parametro < '0' || *parametro > '9')
      {
         return 0; // tiene un caracter no numerico
      }
      parametro++;
   }
   return 1; // es un numero
}


int char_a_numero(char *parametro)
{
   return atoi(parametro);
}

