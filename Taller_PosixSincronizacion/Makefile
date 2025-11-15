######################################################################################
#       Autor: Nicolas Pinilla Ardila
#       Profesor: J. Corredor
#       Fecha: 14 de Noviembre 2025
#       Asingacion: Taller 03 Posix Sincronizacion
#       Descripción:
#     - Este Makefile automatiza la compilación del taller, generando los ejecutables
#     - de la Actividad 1 y Actividad 2.
########################################################################################

CC = gcc                     # Compilador usado
CFLAGS = -Wall -pthread -lrt # Flags: warnings, hilos POSIX y librería tiempo real

# Nombres de los ejecutables
PRODUCER = producer
CONSUMER = consumer
POSIX = posixSincro

# Archivos fuente asociados a cada ejecutable
PRODUCER_SRC = producer.c
CONSUMER_SRC = consumer.c
POSIX_SRC = posixSincro.c posixSincroFunciones.c

# Compila todas las actividades del taller
all: actividad1 actividad2

# Actividad 1: productor y consumidor
actividad1: $(PRODUCER) $(CONSUMER)

# Actividad 2: sincronización con hilos
actividad2: $(POSIX)

# Regla para compilar producer.c
$(PRODUCER): $(PRODUCER_SRC) header.h
	$(CC) $(CFLAGS) $(PRODUCER_SRC) -o $(PRODUCER)

# Regla para compilar consumer.c
$(CONSUMER): $(CONSUMER_SRC) header.h
	$(CC) $(CFLAGS) $(CONSUMER_SRC) -o $(CONSUMER)

# Regla para compilar posixSincro (actividad 2)
$(POSIX): $(POSIX_SRC) posixSincro.h
	$(CC) $(CFLAGS) $(POSIX_SRC) -o $(POSIX)

# Limpieza de ejecutables creados
clean:
	rm -f $(PRODUCER) $(CONSUMER) $(POSIX)
