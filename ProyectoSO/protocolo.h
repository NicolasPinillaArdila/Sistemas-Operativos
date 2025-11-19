/*********************************************************************************************
 * Pontificia Universidad Javeriana
 *
 * Autor: Juan Diego Rojas Osorio
 *        Juan José Ballesteros
 *        Nicolas Pinilla Ardila
 * Materia Sistemas Operativos
 * Docente: J. Corredor, PhD
 * Fecha: 27/10/2025
 * Tema: Proyecto Final
 *
 * Descripción:
 * "
 *  Fichero de cabecera con los prototipos de las funciones 
*              y las estructuras de datos utilizadas en el proyecto. 
*              Tanto para el controlador como para los agentes.
 * "
 *********************************************************************************************/


#ifndef PROTOCOLO_H
#define PROTOCOLO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

// Constantes del sistema
#define MIN_HORA 7
#define MAX_HORA 19
#define MAX_PERSONAS 1000
#define MAX_FAMILIAS 100
#define MAX_AGENTES 10
#define TAM_BUFFER 256
#define TIEMPO_ESPERA_AGENTE 2

// Estructuras de datos
typedef struct {
    char nombre_familia[50];
    int hora_solicitada;
    int num_personas;
    int hora_asignada;  // -1 si no se asignó
    int estado;         // 0: pendiente, 1: aprobada, 2: reprogramada, 3: rechazada
    char agente[50];
} Solicitud;

typedef struct {
    char nombre[50];
    int personas_por_hora[MAX_HORA + 1];  // índice 7 am hasta 7 pm o sea 19
    int familias_por_hora[MAX_HORA + 1];
    Solicitud solicitudes[MAX_FAMILIAS];
    int num_solicitudes;
} EstadoParque;

// Mensajes entre procesos
typedef struct {
    char tipo[20];  // "registro", "solicitud", "respuesta", "fin"
    char agente[50];
    char familia[50];
    int hora_solicitada;
    int hora_asignada;
    int num_personas;
    char respuesta[100];
    char pipe_respuesta[100];
} Mensaje;

#endif