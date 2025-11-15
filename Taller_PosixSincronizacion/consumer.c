/*######################################################################################
#       Autor: Nicolas Pinilla Ardila
#       Profesor: J. Corredor
#       Fecha: 14 de Noviembre 2025
#       Asingacion: Taller 03 Posix Sincronizacion
#	Descripción:
#     - El consumidor abre los semáforos creados previamente por el productor.
#     - Accede a la memoria compartida donde se encuentra la estructura compartir_datos.
#     - Espera (bloqueante) a que existan elementos disponibles en el búfer.
#     - Extrae y muestra cada elemento siguiendo un comportamiento FIFO.
#     - Actualiza el índice de lectura del búfer circular.
#     - Señala al productor que se ha liberado un espacio mediante sem_post(vacio).
########################################################################################*/

#include "header.h" // Definición de estructura y constantes compartidas
#include<stdio.h>
#include<pthread.h>

// Función principal del consumidor
int main() {

    // Abrir semáforo que indica espacios libres en el buffer
    sem_t *vacio = sem_open("/vacio", 0);

    // Abrir semáforo que indica elementos listos para consumir
    sem_t *lleno = sem_open("/lleno", 0);

    // Validación de apertura de semáforos
    if (vacio == SEM_FAILED || lleno == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Abrir la memoria compartida creada por el productor
    int fd_compartido = shm_open("/memoria_compartida", O_RDWR, 0644);
    if (fd_compartido < 0) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Mapear la estructura compartir_datos al espacio del proceso
    compartir_datos *compartir = mmap(NULL, sizeof(compartir_datos),
                                      PROT_READ | PROT_WRITE,
                                      MAP_SHARED, fd_compartido, 0);

    // Inicializar el índice de lectura
    compartir->salida = 0;

    // Consumir 10 elementos del buffer compartido
    for (int i = 1; i <= 10; i++) {

        sem_wait(lleno);   // Esperar un elemento disponible

        int item = compartir->bus[compartir->salida]; // Leer dato
        printf("Consumidor: Consume %d\n", item);

        // Avanzar índice de lectura del buffer circular
        compartir->salida = (compartir->salida + 1) % BUFFER;

        sem_post(vacio);   // Notificar que se liberó un espacio

        sleep(2);          // Pausa visual
    }

    // Desmapear memoria y cerrar recursos IPC
    munmap(compartir, sizeof(compartir_datos));
    close(fd_compartido);
    sem_close(lleno);
    sem_unlink("/lleno");
    shm_unlink("/memoria_compartida");

    return 0;
}
