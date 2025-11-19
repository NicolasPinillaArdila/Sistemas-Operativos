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
 *  Declaracipon de las funciones de manejo de pipes para comunicación entre procesos.
 *  Este archivo contiene las declaraciones para la creación, apertura, lectura
 *  y escritura de pipes para poder establecer l comunicación entre el controlador y los agentes. 
*********************************************************************************************/
#ifndef PIPES_H
#define PIPES_H

#include "protocolo.h"

void crear_pipe(const char* nombre);
int abrir_pipe_lectura(const char* nombre);
int abrir_pipe_escritura(const char* nombre);
int escribir_mensaje(int pipe_fd, Mensaje* msg);
int leer_mensaje(int pipe_fd, Mensaje* msg);

#endif