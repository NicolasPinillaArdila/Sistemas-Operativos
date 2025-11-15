#!/usr/bin/perl
#**************************************************************
#          Pontificia Universidad Javeriana
#     Autor: Juan José Ballesteros
#						 Juan Diego Rojas
#						 Nicolas Pinilla
#     Fecha: 
#			Docente: J. Corredor, phD
#     Materia: Sistemas Operativos
#     Tema: Taller de Evaluación de Rendimiento
#     Fichero: script de automatización de ejecución de ejecutables 
#****************************************************************/


$Path = `pwd`;      # Ejecuta el comando 'pwd' y guarda el resultado en $Path
chomp($Path);        # Elimina el salto de línea final que agrega 'pwd'

#---------------------------------------------------------------
# Define el nombre de los ejecutable que se van a probar
@Nombre_Ejecutable = ("clasicaOpenMP", "filasOpenMP", "clasicaPosix", "clasicaFork");

#---------------------------------------------------------------
#  Arreglo con los tamaños de las matrices a usar
# Cada valor representa una matriz cuadrada de NxN
@Size_Matriz = ("50", "100", "500", "1000", "1500");

#---------------------------------------------------------------
# Arreglo con los números de hilos o procesos a probar
# En OpenMP o POSIX, define cuántos hilos usar en cada ejecución
@Num_Hilos = (1, 2, 4, 8, 10);

#---------------------------------------------------------------
# Número de repeticiones de cada caso (para promediar tiempos)
# Por la ley de los grandes números de probabilidad se usa una muestra de 30
$Repeticiones = 30;

#---------------------------------------------------------------
# Ciclo principal de ejecución
#---------------------------------------------------------------
# Se ejecutan todas las combinaciones de tamaño de matriz y número de hilos
# y cada combinación se repite 'n' veces.
foreach $Nombre_Ejecutable (@Nombre_Ejecutable) { #Ciclo para cada ejecutable guardado en @Nombre_Ejecutable
 foreach $size (@Size_Matriz) {           # Para cada tamaño de matriz
    foreach $hilo (@Num_Hilos) {         # Para cada número de hilos

        # Nombre del archivo donde se guardarán los resultados
        $file = "$Path/$Nombre_Ejecutable-" . $size . "-Hilos-" . $hilo . ".dat";

        # Ejecuta el programa repetidas veces
        for ($i = 0; $i < $Repeticiones; $i++) {

            # Ejecuta los programas con cada size e hilo
            system("$Path/$Nombre_Ejecutable $size $hilo >> $file");

            
            #printf("$Path/$Nombre_Ejecutable $size $hilo \n");
        }

        # Cierra el archivo 
        close($file);
        $p = $p + 1;  
    }
  }
}