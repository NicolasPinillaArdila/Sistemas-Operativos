/*********************************************************************************************
 * Pontificia Universidad Javeriana
 *
 * Autor: Juan Diego Rojas Osorio
 *        Juan José Ballesteros
 *        Nicolas Pinilla Ardila
 * Materia Sistemas Operativos
 * Docente: J. Corredor, PhD
 * Fecha: 27/10/2025
 * Tema: Proyecto Final
 *
 * Descripción:
 *  Implementación de las funciones de manejo de pipes para comunicación entre procesos.
 *  Este archivo contiene las operaciones básicas para la creación, apertura, lectura
 *  y escritura de pipes nominales que permiten la comunicación entre el
 *  controlador y los agentes. 
 *********************************************************************************************/
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "pipes.h"

void crear_pipe(const char* nombre) {
    if (mkfifo(nombre, 0666) < 0) {
        perror("Error creando pipe");
    }
}

int abrir_pipe_lectura(const char* nombre) {
    int intentarAbrir = open(nombre, O_RDONLY | O_NONBLOCK);
    if (intentarAbrir < 0) {
        perror("Error abriendo pipe para lectura");
    }
    return intentarAbrir;
}

int abrir_pipe_escritura(const char* nombre) {
    int intentarAbrir = open(nombre, O_WRONLY);
    if (intentarAbrir < 0) {
        perror("Error abriendo pipe para escritura");
    }
    return intentarAbrir;
}

int escribir_mensaje(int pipe_fd, Mensaje* msg) {
    return write(pipe_fd, msg, sizeof(Mensaje));
}

int leer_mensaje(int pipe_fd, Mensaje* msg) {
    return read(pipe_fd, msg, sizeof(Mensaje));
}