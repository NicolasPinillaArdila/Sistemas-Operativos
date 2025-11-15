/*#######################################################################################
#* Fecha:
#* Autores:
#*       Juan José Ballesteros
#*       Juan Diego Rojas Osorio
#*       Nicolas Pinilla Ardila
#* Programa:
#*      .c implementación de funciones para manejo y multiplicación de matrices con Pthreads
#* Versión:
#*      Paralelismo con Hilos Pthreads "Posix" 
######################################################################################*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "tiempo.h"
#include "mmInterfazPosix.h"

//Variables globales
pthread_mutex_t MM_mutex;
double *matrixA, *matrixB, *matrixC;


//Función para inicializar matrices
void iniMatrix(double *m1, double *m2, int D){ 
  for(int i = 0; i < D*D; i++, m1++, m2++){
      *m1 = (double)rand()/RAND_MAX*(5.0-1.0); 
      *m2 = (double)rand()/RAND_MAX*(9.0-5.0); 
    }	
}

//Función para imprimir matrices
void impMatrix(double *matriz, int D){
  if(D < 9){
        for(int i = 0; i < D*D; i++){
            if(i%D==0) printf("\n");
                printf(" %.2f ", matriz[i]);
      }	
      printf("\n>-------------------->\n");
  }
}

//Función para multiplicar matrices
void *multiMatrix(void *variables){
  
  //Se reciben los parametros pasados a la función
  struct parametros *data = (struct parametros *)variables;

  //Se inicializan las variables
  int idH		= data->idH;
  int nH		= data->nH;
  int D		= data->N;
  int filaI	= (D/nH)*idH;
  int filaF	= (D/nH)*(idH+1);
  double Suma, *pA, *pB;

  //Se multiplican las matrices
    for (int i = filaI; i < filaF; i++){ //Se multiplica la fila i de la matriz A por todas las columnas de la matriz B
        for (int j = 0; j < D; j++){
      pA = matrixA + i*D; 
      pB = matrixB + j;
      Suma = 0.0;
            for (int k = 0; k < D; k++, pA++, pB+=D){ //Se multiplica la fila i de la matriz A por la columna j de la matriz B
        Suma += *pA * *pB;
      }
      matrixC[i*D+j] = Suma;
    }
  }

  //Se libera la memoria de los parametros
  pthread_mutex_lock (&MM_mutex);
  pthread_mutex_unlock (&MM_mutex);
  pthread_exit(NULL);
}
