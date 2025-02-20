#ifndef UTILS_PROTOCOL_REGISTERS_H
#define UTILS_PROTOCOL_REGISTERS_H

#include <stdlib.h>

typedef struct
{
   u_int8_t AX;
   u_int8_t BX;
   u_int8_t CX;
   u_int8_t DX;
   u_int32_t EAX;
   u_int32_t EBX;
   u_int32_t ECX;
   u_int32_t EDX;
   u_int32_t SI;
   u_int32_t DI;

} t_registers;

t_registers crear_registros();

#endif // UTILS_PROTOCOL_REGISTROS_H