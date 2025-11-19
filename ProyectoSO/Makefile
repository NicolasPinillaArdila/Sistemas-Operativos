###################################################################################
#		Pontificia Universidad Javeriana
#	Autor: Juan José Ballesteros
#	       Juan Diego Rojas
#        Nicolas Pinilla
#	Materia: Sistemas Operativos
#	Descripcion:  Archivo Makefile para la automatizacion de compilacion de los programas utilizados en este proyecto
###############################################################################

GCC = gcc
CFLAGS = -Wall
LDFLAGS = -pthread

all: controlador agente

# Programa: Controlador del sistema de reservas
controlador: controlador.c pipes.c log.c
	$(GCC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Programa: Agente solicitante de reservas
agente: agente.c pipes.c log.c
	$(GCC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f controlador agente