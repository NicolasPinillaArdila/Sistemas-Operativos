/*#######################################################################################
 #* Fecha: 
 #* Autores: 
 #* 	 Juan José Ballesteros
 #* 	 Juan Diego Rojas Osorio
 #* 	 Nicolas Pinilla Ardila
 #* Programa: 
 #* 	 .c implementación de funciones para manejo y multiplicación de matrices mediante transpuesta con filas 
 #*     usando OpenMP
 #* Versión:
 #*	 Paralelismo con OpenMP
######################################################################################*/


#include "mmInterfazFilasOMP.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Funcion para imprimir las matrices, esta vez dependiendo del tipo (filas o columnas)
void impMatrix(double *matrix, int D, int t){
  int aux = 0;
  if(D < 6)  //Se imprime solo si la matriz es menor a 6x6
    switch(t){
      //Se imprime por filas
      case 0:  
        for(int i=0; i<D*D; i++){
          if(i%D==0) printf("\n");
            printf("%.2f ", matrix[i]);
        }
        printf("\n  - \n");
        break;
      //Se imprime por columnas
      case 1:
        while(aux<D){
          for(int i=aux; i<D*D; i+=D)
            printf("%.2f ", matrix[i]);
          aux++;
          printf("\n");
        }	
        printf("\n  - \n");
        break;
      default:
        printf("Sin tipo de impresión\n");
    }
}

//Funcion para inicializar las matrices m1 y m2 con valores aleatorios
void iniMatrix(double *m1, double *m2, int D){
  for(int i=0; i<D*D; i++, m1++, m2++){
    *m1 = (double)rand()/RAND_MAX*(5.0-1.0);	
    *m2 = (double)rand()/RAND_MAX*(9.0-5.0);		
  }
}

//Funcion para multiplicar dos matrices cuadradas mA y mB, esta vez se asume que mB es transpuesta con el fin de mejorar el rendimiento, se utiliza OpenMP para paralelizar el proceso
void multiMatrixTrans(double *mA, double *mB, double *mC, int D){
  double Suma, *pA, *pB;
  //Región paralela OpenMP
  #pragma omp parallel
  {
  //Division del bucle entre los hilos, donde cada hilo calcula una parte de la matriz resultante
  #pragma omp for
  for(int i=0; i<D; i++){
    for(int j=0; j<D; j++){
      pA = mA+i*D;	//Se posiciona en la fila i de la matriz A
      pB = mB+j*D;	//Se posiciona en la fila j de la matriz B
      Suma = 0.0;
      for(int k=0; k<D; k++, pA++, pB++){
        Suma += *pA * *pB;
      }
      mC[i*D+j] = Suma;  //Se almacena el resultado en la matriz C
    }
  }
  }
}
