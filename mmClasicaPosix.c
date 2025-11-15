/*#######################################################################################
#* Fecha:
#* Autores:
#*       Juan José Ballesteros
#*       Juan Diego Rojas Osorio
#*       Nicolas Pinilla Ardila
#* Programa:
#*      Multiplicación de Matrices algoritmo clásico de Pthreads, funcion principal 
#*			que llama a las funciones de mmInterfazPosix.c
#* Versión:
#*      Paralelismo con Hilos Pthreads "Posix" 
######################################################################################*/

#include "tiempo.h"
#include "mmInterfazPosix.h"

/*
 * Función principal que:
 * 1. Valida argumentos de entrada (tamaño y número de hilos)
 * 2. Reserva memoria e inicializa matrices
 * 3. Crea hilos para multiplicar matrices
 * 4. Mide el tiempo total
 * 5. Imprime resultados si el tamaño es pequeño
 */
int main(int argc, char *argv[]){
	if (argc < 3){
		printf("Ingreso de argumentos \n $./ejecutable tamMatriz numHilos\n");
		exit(0);	
	}
	//Se reciben los parametros pasados por consola
    int N = atoi(argv[1]); 
    int n_threads = atoi(argv[2]); 

	//Se inicializan las variables
    pthread_t p[n_threads];
    pthread_attr_t atrMM;

	//Se reserva memoria para las matrices
	matrixA  = (double *)calloc(N*N, sizeof(double));
	matrixB  = (double *)calloc(N*N, sizeof(double));
	matrixC  = (double *)calloc(N*N, sizeof(double));

	//Se inicializa la matriz y se imprimen las matrices
	iniMatrix(matrixA, matrixB, N);
	impMatrix(matrixA, N);
	impMatrix(matrixB, N);

	//Se inicia la medición del tiempo
	InicioMuestra();

	//Se inicializan los hilos
	pthread_mutex_init(&MM_mutex, NULL);
	pthread_attr_init(&atrMM);
	pthread_attr_setdetachstate(&atrMM, PTHREAD_CREATE_JOINABLE);

	//Se crean los hilos y se les asignan los parametros
    for (int j=0; j<n_threads; j++){
		struct parametros *datos = (struct parametros *) malloc(sizeof(struct parametros)); 
		datos->idH = j;
		datos->nH  = n_threads;
		datos->N   = N;

			//Se crean los hilos
        pthread_create(&p[j],&atrMM,multiMatrix,(void *)datos);
	}

	//Se esperan a que los hilos terminen
    for (int j=0; j<n_threads; j++)
        pthread_join(p[j],NULL);

	//Se termina la medición del tiempo
	FinMuestra();

	//Se imprime la matriz resultante de la multiplicación
	impMatrix(matrixC, N);

	/*Liberación de Memoria*/
	free(matrixA);
	free(matrixB);
	free(matrixC);

	//Se liberan los hilos
	pthread_attr_destroy(&atrMM);
	pthread_mutex_destroy(&MM_mutex);
	pthread_exit (NULL);

	return 0;
}

