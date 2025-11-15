/*#######################################################################################
#       Autor: Nicolas Pinilla Ardila
#       Profesor: J. Corredor
#       Fecha: 14 de Noviembre 2025
#       Asingacion: Taller 03 Posix Sincronizacion
#       Descripción:
#     - Implementa la lógica principal de sincronización usando hilos POSIX: crea los
#     - productores y el spooler, coordina el acceso al búfer y controla la ejecución
#     - hasta que todos los hilos terminan.
#######################################################################################*/

//agrega el encabezado de las constantes variables para el correcto funcionamiento
#include "posixSincro.h"

int main() {

    // Declaración del hilo spooler y del arreglo de hilos productores
    pthread_t tid_producer[NUM_PRODUCERS], tid_spooler;

    // IDs numéricos que se pasan a cada productor
    int thread_no[NUM_PRODUCERS];
    int r;

    // Crear el hilo spooler (encargado de imprimir las líneas)
    if ((r = pthread_create(&tid_spooler, NULL, spooler, NULL)) != 0) {
        fprintf(stderr, "Error creando spooler: %s\n", strerror(r));
        exit(1);
    }

    // Crear los hilos productores
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        thread_no[i] = i; // ID único para cada productor

        if ((r = pthread_create(&tid_producer[i], NULL, producer, &thread_no[i])) != 0) {
            fprintf(stderr, "Error creando productor %d: %s\n", i, strerror(r));
            exit(1);
        }
    }

    // Esperar a que todos los productores terminen su ejecución
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(tid_producer[i], NULL);
    }

    // Esperar a que el spooler vacíe todas las líneas en el buffer
    while (1) {
        pthread_mutex_lock(&buf_mutex);

        if (lines_to_print == 0) {  // Si no quedan líneas, se puede salir
            pthread_mutex_unlock(&buf_mutex);
            break;
        }

        pthread_mutex_unlock(&buf_mutex);
        sleep(1); // Pausa corta mientras espera
    }

    // Cancelar y unir el hilo spooler una vez terminado el trabajo
    pthread_cancel(tid_spooler);
    pthread_join(tid_spooler, NULL);

    // Mensaje final
    printf("\nLos hilos terminaron correctamente.\n");

    return 0; // Fin del programa
}
