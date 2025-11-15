###################################################################################
#		Pontificia Universidad Javeriana
#	Autor: Juan José Ballesteros
#	       Juan Diego Rojas
#	       Nicolas Pinilla
#	Materia: Sistemas Operativos
#	Fichero: Automatizacion de compilación
###############################################################################

GCC = gcc
CFLAGS = -fopenmp -Wall
LDFLAGS = -lpthread

all: clasicaOpenMP filasOpenMP clasicaPosix clasicaFork

clasicaOpenMP: mmClasicaOpenMP.c mmInterfazClasicaOMP.c tiempo.c
	$(GCC) $(CFLAGS) $^ -o $@

filasOpenMP: mmFilasOpenMP.c mmInterfazFilasOMP.c tiempo.c
	$(GCC) $(CFLAGS) $^ -o $@

clasicaPosix: mmClasicaPosix.c mmInterfazPosix.c tiempo.c
	$(GCC) $^ -o $@ $(LDFLAGS)

clasicaFork: mmClasicaFork.c mmInterfazFork.c tiempo.c
	$(GCC) $^ -o $@

clean:
	rm -f clasicaOpenMP filasOpenMP clasicaPosix clasicaFork
