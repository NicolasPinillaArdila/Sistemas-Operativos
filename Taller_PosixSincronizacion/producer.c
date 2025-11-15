/*#######################################################################################
#       Autor: Nicolas Pinilla Ardila
#       Profesor: J. Corredor
#       Fecha: 14 de Noviembre 2025
#       Asingacion: Taller 03 Posix Sincronizacion
#       Descripción:
#     - Implementa el hilo productor que escribe datos en un búfer compartido, sincronizando el acceso mediante semáforos y memoria compartida POSIX.
########################################################################################*/

// Incluye la estructura y constantes del sistema de memoria compartida
#include "header.h"

int main() {

    // Crear semáforo que controla espacios libres en el buffer
    sem_t *vacio = sem_open("/vacio", O_CREAT, 0644, BUFFER);

    // Crear semáforo que controla elementos disponibles para consumir
    sem_t *lleno = sem_open("/lleno", O_CREAT, 0644, 0);

    // Validar apertura de semáforos
    if (vacio == SEM_FAILED || lleno == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Crear o abrir segmento de memoria compartida
    int shm_fd = shm_open("/memoria_compartida", O_CREAT | O_RDWR, 0644);
    if (shm_fd < 0) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Ajustar tamaño del segmento al tamaño de la estructura
    ftruncate(shm_fd, sizeof(compartir_datos));

    // Mapear la memoria compartida al espacio del proceso
    compartir_datos *compartir = mmap(NULL, sizeof(compartir_datos),
                                      PROT_READ | PROT_WRITE,
                                      MAP_SHARED, shm_fd, 0);

    // Inicializar el índice de escritura del buffer circular
    compartir->entrada = 0;

    // Generar 10 elementos del productor
    for (int i = 1; i <= 10; i++) {

        sem_wait(vacio);  // Esperar espacio disponible en el buffer

        compartir->bus[compartir->entrada] = i;  // Escribir dato
        printf("Productor: Produce %d\n", i);

        // Avanzar índice circular de escritura
        compartir->entrada = (compartir->entrada + 1) % BUFFER;

        sem_post(lleno);  // Señalar que hay un elemento nuevo

        sleep(1);         // Espera breve (visualizar ejecución)
    }

    // Liberar memoria compartida y cerrar semáforos
    munmap(compartir, sizeof(compartir_datos));
    close(shm_fd);
    sem_close(vacio);
    sem_unlink("/vacio");
    shm_unlink("/memoria_compartida");

    return 0;
}
