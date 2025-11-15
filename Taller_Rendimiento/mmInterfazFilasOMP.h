/*#######################################################################################
 #* Fecha: 
 #* Autores: 
 #* 	 Juan José Ballesteros
 #* 	 Juan Diego Rojas Osorio
 #* 	 Nicolas Pinilla Ardila
 #* Programa: 
 #* 	 .h declaraciones de funciones para manejo y multiplicación de matrices mediante transpuesta con filas
 #*     usando OpenMP
 #* Versión:
 #*	 Paralelismo con OpenMP
######################################################################################*/

#ifndef MATRICES_FILAS_OMP_H
#define MATRICES_FILAS_OMP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>

//Funcion para imprimir las matrices, esta vez dependiendo del tipo (filas o columnas)
void impMatrix(double *matrix, int D, int t);

//Funcion para inicializar las matrices m1 y m2 con valores aleatorios
void iniMatrix(double *m1, double *m2, int D);

//Funcion para multiplicar dos matrices cuadradas mA y mB, esta vez se asume que mB es transpuesta con el fin de mejorar el rendimiento, 
//se utiliza OpenMP para paralelizar el proceso
void multiMatrixTrans(double *mA, double *mB, double *mC, int D);

#endif