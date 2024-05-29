#include "servidor.h"

void iniciar_servidor()
{

    char *puerto_escucha = get_puerto_escucha();

    int32_t *fd_escucha = malloc(sizeof(int32_t));
    *fd_escucha = crear_servidor(puerto_escucha);

    pthread_t hilo_escucha;
    pthread_create(&hilo_escucha, NULL, &escuchar_conexiones, fd_escucha);
    pthread_join(hilo_escucha, NULL); // Esto lo cambie a join para que el hilo sea bloqueante
}

void *escuchar_conexiones(void *fd_escucha)
{
    while (1)
    {
        esperar_cliente(*((int32_t *)fd_escucha), &atender_cliente);
    }

    return NULL;
}

void *atender_cliente(void *fd_ptr)
{
    int32_t fd_conexion = *((int32_t *)fd_ptr);

    // atender handsake (para saber quienes el cliente)
    int32_t modulo_cliente = recibir_cliente(fd_conexion);

    switch (modulo_cliente)
    {
    case KERNEL:
        escuchar_kernel(fd_conexion);
        break;
    case CPU:
        escuchar_cpu(fd_conexion);
        break;
    case E_S:
        escuchar_interfaz_es(fd_conexion);
        break;
    default:
        printf("Error de Cliente \n");
        return NULL;
    }

    return NULL;
}

void escuchar_kernel(int32_t fd_kernel)
{
    printf("Kernel conectado \n");

    t_kernel_mem_req *mem_request = recibir_kernel_mem_request(fd_kernel);

    switch (mem_request->tipo)
    {
    case INICIAR_PROCESO:
        printf("INICIAR_PROCESO \n");
        cargar_proceso_a_memoria(mem_request->pid, mem_request->parametros.path);
        break;

    case FINALIZAR_PROCESO:
        printf("FINALIZAR_PROCESO \n");
        break;

    default:
        printf("Error de instruccion \n");
        break;
    }
}

void escuchar_cpu(int32_t fd_cpu)
{
    printf("CPU conectado \n");
    recibir_mensaje(fd_cpu);

    recibir_mensaje(fd_cpu); // block
}

void escuchar_interfaz_es(int32_t fd_es)
{
    printf("Interfaz E/S conectada \n");
    recibir_mensaje(fd_es);
}
