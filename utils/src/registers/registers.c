#include "registers.h"

t_registers crear_registros()
{
   t_registers registros;

   registros.AX = 0;
   registros.BX = 0;
   registros.CX = 0;
   registros.DX = 0;
   registros.EAX = 0;
   registros.EBX = 0;
   registros.ECX = 0;
   registros.EBX = 0;
   registros.SI = 0;
   registros.DI = 0;

   return registros;
}
