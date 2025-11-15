/*#######################################################################################
#* Materia: Sistemas Operativos
#* Tema: Paralelismo con Fork
#* Autor: Juan José Ballesteros
#*       Juan Diego Rojas Osorio
#*       Nicolas Pinilla Ardila
#* Fecha:
#* Docente: J. Corredor, PhD
#* Programa:
#*      .c implementación de funciones para manejo y multiplicación de matrices con Fork
#* Versión:
#*      Paralelismo con Procesos Fork
######################################################################################*/

#include "mmInterfazFork.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


//Función para multiplicar matrices
void multiMatrix(double *mA, double *mB, double *mC, int D, int filaI, int filaF) {
    double Suma, *pA, *pB;

    // Multiplicación de matrices
    // Cada proceso multiplica una fila de la matriz A por todas las columnas de la matriz B
    for (int i = filaI; i < filaF; i++) {
        for (int j = 0; j < D; j++) {
            Suma = 0.0;
            pA = mA+i*D;
            pB = mB+j;
            for (int k = 0; k < D; k++, pA++, pB+=D) {
                Suma += *pA * *pB;	
            }
            mC[i*D+j] = Suma;
        }
    }
}

//Función para imprimir matrices
void impMatrix(double *matrix, int D) {
    if (D < 9) {
        printf("\nImpresión	...\n");

        for (int i = 0; i < D*D; i++, matrix++) {
            if(i%D==0) printf("\n");
                printf(" %.2f ", *matrix);
            }
        printf("\n ");
    }
}

//Función para inicializar matrices
void iniMatrix(double *mA, double *mB, int D){
    for (int i = 0; i < D*D; i++, mA++, mB++){
            *mA = (double)rand()/RAND_MAX*(5.0-1.0); ; 
            *mB = (double)rand()/RAND_MAX*(9.0-5.0); 
        }
}