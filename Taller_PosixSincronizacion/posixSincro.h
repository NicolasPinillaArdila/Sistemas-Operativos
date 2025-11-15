/*#######################################################################################
#       Autor: Nicolas Pinilla Ardila
#       Profesor: J. Corredor
#       Fecha: 14 de Noviembre 2025
#       Asingacion: Taller 03 Posix Sincronizacion
#       Descripción:
#     - Define las constantes, variables globales y prototipos necesarios para la
#     - sincronización entre hilos productores y el spooler usando mutex y variables de condición POSIX.
########################################################################################*/

#ifndef POSIXSINCRO_H
#define POSIXSINCRO_H

// Librerías necesarias para hilos, E/S y funciones básicas
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

// Tamaño del buffer circular y número de hilos productores
#define MAX_BUFFERS 5
#define NUM_PRODUCERS 10
#define NUM_ITEMS 10

// Variables globales compartidas entre productores y spooler
extern char buf[MAX_BUFFERS][100];      // Buffer circular de líneas
extern int buffer_index;                // Posición para escribir
extern int buffer_print_index;          // Posición para imprimir
extern pthread_mutex_t buf_mutex;       // Mutex de sincronización
extern pthread_cond_t buf_cond;         // Condición: buffer libre
extern pthread_cond_t spool_cond;       // Condición: línea lista
extern int buffers_available;           // Espacios disponibles
extern int lines_to_print;              // Líneas pendientes

// Prototipos de los hilos productor y spooler
void *producer(void *arg);
void *spooler(void *arg);

#endif
