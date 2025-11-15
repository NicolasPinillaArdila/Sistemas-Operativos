/*#######################################################################################
 #* Fecha: 
 #* Autores: 
 #* 	 Juan José Ballesteros
 #* 	 Juan Diego Rojas Osorio
 #* 	 Nicolas Pinilla Ardila
 #* Programa: 
 #* 	 .h declaraciones de funciones para manejo y multiplicación de matrices con OpenMP
 #* Versión:
 #*	 Paralelismo con OpenMP
######################################################################################*/

#ifndef MATRICES_CLASICA_OMP_H
#define MATRICES_CLASICA_OMP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>

// Inicializa matrices A y B con valores aleatorios
void iniMatrix(double *mA, double *mB, int D);

// Imprime una matriz (si es pequeña)
void impMatrix(double *matrix, int D);

// Multiplica las matrices A y B en el rango de filas asignado
void multiMatrix(double *mA, double *mB, double *mC, int D);

#endif
