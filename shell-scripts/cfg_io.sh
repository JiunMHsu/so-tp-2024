#!/bin/bash

bash config-setup/_kernel_setup.sh
bash config-setup/_memoria_setup.sh
bash config-setup/_cpu_setup.sh
bash config-setup/_entradasalida_setup.sh GENERICA TECLADO MONITOR
cd ..

# Kernel
cd kernel/
echo "ALGORITMO_PLANIFICACION=RR" >> kernel.config
echo "QUANTUM=750" >> kernel.config
echo "RECURSOS=[REC1]" >> kernel.config
echo "INSTANCIAS_RECURSOS=[1]" >> kernel.config
echo "GRADO_MULTIPROGRAMACION=10" >> kernel.config
cd ..

# Memoria
cd memoria/
echo "TAM_MEMORIA=1024" >> memoria.config
echo "TAM_PAGINA=16" >> memoria.config
echo "RETARDO_RESPUESTA=100" >> memoria.config
cd ..

# CPU
cd cpu/
echo "CANTIDAD_ENTRADAS_TLB=0" >> cpu.config
echo "ALGORITMO_TLB=FIFO" >> cpu.config
cd ..

cd entradasalida/

# GENERICA
echo "TIPO_INTERFAZ=GENERICA" >> GENERICA.config
echo "TIEMPO_UNIDAD_TRABAJO=250" >> GENERICA.config

# TECLADO
echo "TIPO_INTERFAZ=STDIN" >> TECLADO.config
echo "TIEMPO_UNIDAD_TRABAJO=250" >> TECLADO.config

# MONITOR
echo "TIPO_INTERFAZ=STDOUT" >> MONITOR.config
echo "TIEMPO_UNIDAD_TRABAJO=250" >> MONITOR.config
