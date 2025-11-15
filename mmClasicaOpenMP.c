/*#######################################################################################
 #* Fecha: 
 #* Autores: 
 #* 	 Juan José Ballesteros
 #* 	 Juan Diego Rojas Osorio
 #* 	 Nicolas Pinilla Ardila
 #* Programa: 
 #* 	 Multiplicación de Matrices algoritmo clásico
 #* Versión:
 #*	 Paralelismo con OpenMP
######################################################################################*/


#include "mmInterfazClasicaOMP.h"
#include "tiempo.h"

/*
 * Función principal que:
 * 1. Valida argumentos de entrada (tamaño y número de hilos)
 * 2. Reserva memoria e inicializa matrices
 * 3. Realiza la multiplicación de matrices en paralelo 
 * 4. Mide el tiempo de ejecución
 * 5. Imprime las matrices si son pequeñas
 */
int main(int argc, char *argv[]){
	
	//Verificacion de los argumentos de entrada
	if(argc < 3){
		printf("\n Use: $./clasicaOpenMP SIZE Hilos \n\n");
		exit(0);
	}

	//Se reciben los argumentos de entrada
	int N = atoi(argv[1]);  // Tamaño de la matriz
	int TH = atoi(argv[2]);  // Numero de hilos

	//Se asigna memoria dinamica para las matrices
	double *matrixA  = (double *)calloc(N*N, sizeof(double));
	double *matrixB  = (double *)calloc(N*N, sizeof(double));
	double *matrixC  = (double *)calloc(N*N, sizeof(double));

	//Se generan los numeros aleatorios
	srand(time(NULL));

	//Se fija el numero de hilos
	omp_set_num_threads(TH);

	//Se inicializan las matrices
	iniMatrix(matrixA, matrixB, N);

	//Se imprimen las matrices
	impMatrix(matrixA, N);
	impMatrix(matrixB, N);

	//Se registra el tiempo inicial y se ejecuta el algoritmo
	InicioMuestra();
	multiMatrix(matrixA, matrixB, matrixC, N);
	FinMuestra();

	//Se imprime la matriz resultante de la multiplicacion de mA y mB
	impMatrix(matrixC, N);

	/*Liberación de Memoria*/
	free(matrixA);
	free(matrixB);
	free(matrixC);
	
	return 0;
}
