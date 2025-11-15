/*#######################################################################################
#       Autor: Nicolas Pinilla Ardila
#       Profesor: J. Corredor
#       Fecha: 14 de Noviembre 2025
#       Asingacion: Taller 03 Posix Sincronizacion
#       Descripción:
#     - Implementa las funciones producer y spooler, encargadas de generar mensajes y
#     - consumirlos desde un búfer sincronizado mediante mutex y variables de condición POSIX.
#######################################################################################*/

#include "posixSincro.h"

// Búfer circular donde los productores escriben mensajes
char buf[MAX_BUFFERS][100];

// Índices para escribir y leer en el búfer
int buffer_index = 0;
int buffer_print_index = 0;

// Mutex y variables de condición para sincronizar acceso al búfer
pthread_mutex_t buf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buf_cond = PTHREAD_COND_INITIALIZER;     // Señala espacio libre
pthread_cond_t spool_cond = PTHREAD_COND_INITIALIZER;    // Señala línea lista

// Contadores del estado del búfer
int buffers_available = MAX_BUFFERS; // Espacios libres
int lines_to_print = 0;              // Líneas pendientes para imprimir

// Función productor: genera mensajes y los coloca en el búfer compartido
void *producer(void *arg) {

    int my_id = *((int *)arg); // ID del productor
    int count = 0;             // Contador de mensajes producidos

    for (int i = 0; i < NUM_ITEMS; i++) {

        pthread_mutex_lock(&buf_mutex); // Bloquear acceso al búfer

        // Esperar si el búfer está lleno
        while (buffers_available == 0)
            pthread_cond_wait(&buf_cond, &buf_mutex);

        // Insertar el mensaje en la posición actual del búfer
        int j = buffer_index;
        sprintf(buf[j], " Hilo productor %d produjo %d\n", my_id, ++count);

        // Avanzar índice circular del búfer
        buffer_index = (buffer_index + 1) % MAX_BUFFERS;

        buffers_available--;  // Se ocupa un espacio
        lines_to_print++;     // Nueva línea lista para imprimir

        // Avisar al spooler que hay datos disponibles
        pthread_cond_signal(&spool_cond);

        pthread_mutex_unlock(&buf_mutex); // Liberar el búfer

        sleep(1); // Pausa de simulación
    }

    return NULL;
}

// Función spooler: actúa como consumidor e imprime los mensajes del búfer
void *spooler(void *arg) {

    while (1) {

        pthread_mutex_lock(&buf_mutex); // Bloquear acceso al búfer

        // Esperar si no hay líneas disponibles para imprimir
        while (lines_to_print == 0)
            pthread_cond_wait(&spool_cond, &buf_mutex);

        // Imprimir la siguiente línea del búfer
        printf(" Spooler imprime: %s", buf[buffer_print_index]);

        // Avanzar índice circular de lectura
        buffer_print_index = (buffer_print_index + 1) % MAX_BUFFERS;

        lines_to_print--;     // Se procesó una línea
        buffers_available++;  // Se libera un espacio en el búfer

        // Avisar a los productores que hay espacio libre
        pthread_cond_signal(&buf_cond);

        pthread_mutex_unlock(&buf_mutex); // Liberar el búfer

        sleep(1); // Pausa visual de impresión
    }

    return NULL;
}
