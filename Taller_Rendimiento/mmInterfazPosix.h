/*#######################################################################################
#* Fecha:
#* Autores:
#*       Juan José Ballesteros
#*       Juan Diego Rojas Osorio
#*       Nicolas Pinilla Ardila
#* Programa:
#*      .h declaraciones de funciones para manejo y multiplicación de matrices con Pthreads
#* Versión:
#*      Paralelismo con Hilos Pthreads "Posix" 
######################################################################################*/


#ifndef _MMPOSIX_H
#define _MMPOSIX_H

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "tiempo.h"



//Variables globales
extern pthread_mutex_t MM_mutex;
extern double *matrixA, *matrixB, *matrixC;

//Estructura para pasar parametros a la función multiMatrix
struct parametros{
  int nH;
  int idH;
  int N;
};

//Función para inicializar matrices
void iniMatrix(double *m1, double *m2, int D);

//Función para imprimir matrices
void impMatrix(double *matriz, int D);

//Función para multiplicar matrices
void *multiMatrix(void *variables);

#endif

