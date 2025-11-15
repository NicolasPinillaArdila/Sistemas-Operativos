/*#######################################################################################
 #* Fecha:
 #* Autores:
 #*      Juan José Ballesteros
 #*      Juan Diego Rojas Osorio
 #*      Nicolas Pinilla Ardila
 #* Programa:
 #*      Multiplicación de Matrices algoritmo matriz Transpuesta (Filas x Filas) 
 #* Versión:
 #*      Paralelismo con OpenMP
 #######################################################################################*/

#include "mmInterfazFilasOMP.h"
#include "tiempo.h"

/*
 * Función principal en la que:
 * 1. Se verifica que se ingresen correctamente los argumentos (tamaño de matriz y número de hilos)
 * 2. Se reserva memoria dinámica para las tres matrices (matrixA, matrixB, matrixC)
 * 3. Se inicializan las matrices A y B con valores aleatorios
 * 4. Se establece el número de hilos que OpenMP utilizará para la ejecución paralela
 * 5. Se imprimen las matrices A y B si su tamaño es pequeño, en este caso menor a 6x6
 * 6. Se inicia la medición del tiempo de ejecución
 * 7. Se realiza la multiplicación de matrices usando la versión transpuesta de B y paralelismo con OpenMP
 * 8. Se finaliza la medición del tiempo e imprime el resultado en microsegundos
 * 9. Se imprime la matriz resultante C 
 * 10. Se libera la memoria dinámica utilizada por las tres matrices
 */
int main(int argc, char *argv[]){
	//Verificacion de los argumentos de entrada
	if(argc < 3){
		printf("\n Use: $./clasicaOpenMP SIZE Hilos \n\n");
		exit(0);
	}

	//Se reciben los argumentos de entrada
	int N = atoi(argv[1]);   // Tamaño de la matriz
	int TH = atoi(argv[2]);  // Numero de hilos

	//Se asigna memoria dinamica para las matrices
	double *matrixA  = (double *)calloc(N*N, sizeof(double));
	double *matrixB  = (double *)calloc(N*N, sizeof(double));
	double *matrixC  = (double *)calloc(N*N, sizeof(double));
	
	srand(time(NULL));  //Se generan los numeros aleatorios

	omp_set_num_threads(TH);  //Se fija el numero de hilos

	iniMatrix(matrixA, matrixB, N);  //Se inicializan las matrices

	//Se imprimen las matrices
	impMatrix(matrixA, N, 0);
	impMatrix(matrixB, N, 1);

	//Se registra el tiempo inicial y se ejecuta el algoritmo
	InicioMuestra();  
	multiMatrixTrans(matrixA, matrixB, matrixC, N);
	FinMuestra();

	//Se imprime la matriz resultante de la multiplicacion de mA y mB
	impMatrix(matrixC, N, 0);

	/*Liberación de Memoria*/
	free(matrixA);
	free(matrixB);
	free(matrixC);
	
	return 0;
}
