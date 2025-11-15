/*#######################################################################################
#* Materia: Sistemas Operativos
#* Tema: Paralelismo con Fork
#* Autor: Juan José Ballesteros
#*       Juan Diego Rojas 
#*       Nicolas Pinilla
#* Fecha:
#* Docente: J. Corredor, PhD
#* Programa:
#*      .h declaraciones de funciones para manejo y multiplicación de matrices con Fork
#* Versión:
#*      Paralelismo con Procesos Fork 
######################################################################################*/

#ifndef _MMFORK_H
#define _MMFORK_H

#include <stdio.h> // Librería estándar de entrada y salida
#include <stdlib.h> // Librería estándar de utilidades
#include <unistd.h> // Librería para funciones de Unix
#include <sys/wait.h> // Librería para funciones de espera de procesos
#include <sys/time.h> // Librería para funciones de tiempo
#include <time.h> // Librería para funciones de tiempo
#include "tiempo.h" // Librería para medir tiempos

// Inicializa matrices A y B con valores aleatorios
void iniMatrix(double *mA, double *mB, int D);

// Imprime una matriz (si es pequeña)
void impMatrix(double *matrix, int D);

// Multiplica las matrices A y B en el rango de filas asignado
void multiMatrix(double *mA, double *mB, double *mC, int D, int filaI, int filaF);

#endif