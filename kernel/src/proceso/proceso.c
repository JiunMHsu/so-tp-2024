#include "proceso.h"

t_pcb new_pcb()
{
   t_pcb pcb;
   pcb.pid = 0;
   pcb.program_counter = 0;
   pcb.quantum = 0;
   return pcb;
}