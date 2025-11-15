#include "mmInterfazClasicaOMP.h"

//Funcion para imprimir las matrices siempre que estas cumplan la condición de ser menores a 9
void impMatrix(double *matrix, int D){
    if(D < 9){
        printf("\n");
        for(int i=0; i<D*D; i++){
            if(i%D==0) printf("\n");
            printf("%.2f ", matrix[i]);
        }
        printf("\n**------------------------------**\n");
    }
}

//Funcion para inicializar las matrices m1 y m2 con valores aleatorios
void iniMatrix(double *m1, double *m2, int D){
    for(int i=0; i<D*D; i++, m1++, m2++){
        *m1 = (double)rand()/RAND_MAX*(5.0-1.0);
        *m2 = (double)rand()/RAND_MAX*(9.0-2.0);
    }
}

// Multiplicación clásica
void multiMatrix(double *mA, double *mB, double *mC, int D) {
    #pragma omp parallel
    {
        #pragma omp for
        for (int i = 0; i < D; i++) {
            for (int j = 0; j < D; j++) {
                double *pA = mA + i * D;
                double *pB = mB + j;
                double Suma = 0.0;
                for (int k = 0; k < D; k++, pA++, pB += D) {
                    Suma += (*pA) * (*pB);
                }
                mC[i * D + j] = Suma;
            }
        }
    }
}
