/*#######################################################################################
#* Archivo: tiempo.c
#* Descripción: Implementación de funciones para medir tiempos con gettimeofday.
######################################################################################*/

#include <stdio.h>
#include <sys/time.h>
#include "tiempo.h"

// Variables globales para medir el tiempo
static struct timeval inicio, fin;

void InicioMuestra() {
    gettimeofday(&inicio, NULL);
}

void FinMuestra() {
    gettimeofday(&fin, NULL);
    fin.tv_usec -= inicio.tv_usec;
    fin.tv_sec -= inicio.tv_sec;
    double tiempo = (double) (fin.tv_sec * 1000000 + fin.tv_usec);
    printf("%9.0f \n", tiempo);
}

