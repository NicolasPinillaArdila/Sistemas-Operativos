/*######################################################################################
#	Autor: Nicolas Pinilla Ardila
#	Profesor: J. Corredor
#       Fecha: 14 de Noviembre 2025
#       Asingacion: Taller 03 Posix Sincronizacion
#       Descripcion: constantes, librerías y estructuras necesarias para la comunicación
#       entre los procesos productor y consumidor mediante memoria compartida POSIX y
#	semáforos con nombre.
########################################################################################*/


#ifndef HEADER_H
#define HEADER_H

// interfaces estándar y específicas de POSIX requeridas para el correcto funcionamiento del programa
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER 5  // Tamaño del búfer circular compartido

// La estructra representa el segmento de memoria compartida entre los procesos productor y consumidor
typedef struct {
    // Búfer circular compartido
    int bus[BUFFER];

    // Posición para insertar (productor)
    int entrada;

    // Posición para retirar (consumidor)
    int salida;
} compartir_datos;

#endif
