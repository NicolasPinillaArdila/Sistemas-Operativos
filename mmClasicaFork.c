/*#######################################################################################
#* Materia: Sistemas Operativos
#* Tema: Paralelismo con Fork
#* Autores: 
#*       Juan José Ballesteros
#*       Juan Diego Rojas Osorio
#*       Nicolas Pinilla Ardila
#* Fecha:
#* Docente: J. Corredor, PhD
#* Programa:
#*      Multiplicación de Matrices algoritmo clásico con Fork
#* Versión:
#*      Paralelismo con Procesos Fork 
######################################################################################*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include "mmInterfazFork.h"
#include "tiempo.h"

/*
 * Función principal en la que:
 * 1. Se verifica que se ingresen correctamente los argumentos (tamaño de matriz y número de procesos)
 * 2. Se inicializan tres matrices dinámicas (matA, matB, matC)
 * 3. Se llenan las matrices matA y matB con valores aleatorios
 * 4. Se imprimen las matrices si su tamaño es menor a 9
 * 5. Se divide el trabajo entre múltiples procesos usando fork()
 * 6. Cada proceso hijo realiza la multiplicación parcial de las matrices
 * 7. Cada proceso hijo imprime su resultado si el tamaño de la matriz es pequeño
 * 8. El proceso padre espera que todos los procesos hijos terminen
 * 9. Se mide e imprime el tiempo total de ejecución
 * 10. Se libera la memoria reservada
 */
int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("\n \t\tUse: $./nom_ejecutable Size Hilos \n");
		exit(0);
	}
    // Se leen los argumentos
	int N      = (int) atoi(argv[1]);
	int num_P  = (int) atoi(argv[2]);
    // Se crean las matrices, se inicializan y se les asigna memoria
	double *matA = (double *) calloc(N*N, sizeof(double));
	double *matB = (double *) calloc(N*N, sizeof(double));
	double *matC = (double *) calloc(N*N, sizeof(double));

    // Se inicializa la para generar números aleatorios
    srand(time(0)); 

    // Se inicializan las matrices
    iniMatrix(matA, matB, N);

    // Se imprimen las matrices
    impMatrix(matA, N);
    impMatrix(matB, N);

    // Se calcula el número de filas por proceso
    int rows_per_process = N/num_P;

    // Se inicia la medición del tiempo
	InicioMuestra();

    // Se crean los procesos hijos
    for (int i = 0; i < num_P; i++) {
        pid_t pid = fork();

        // Si el proceso es el hijo, se ejecuta la función multiMatrix
        if (pid == 0) { 
            int start_row = i * rows_per_process;
            int end_row = (i == num_P - 1) ? N : start_row + rows_per_process;
            
			multiMatrix(matA, matB, matC, N, start_row, end_row); 

            // Se imprimen las filas calculadas por el proceso hijo
			if(N<9){
           		printf("\nChild PID %d calculated rows %d to %d:\n", getpid(), start_row, end_row-1);
            	for (int r = start_row; r < end_row; r++) {
                	for (int c = 0; c < N; c++) {
                    	printf(" %.2f ", matC[N*r+c]);
                	}
                	printf("\n");
            	}
			}
            exit(0); 

            // Si el proceso es el padre, se espera a que terminen los procesos hijos
        } else if (pid < 0) {
            perror("fork failed");
            exit(1);
        }
    }

    // Se espera a que terminen todos los procesos hijos
    for (int i = 0; i < num_P; i++) {
        wait(NULL);
    }

    // Se termina la medición del tiempo
	FinMuestra(); 

    //liberación de memoria
	free(matA);
	free(matB);
	free(matC);

    return 0;
}
