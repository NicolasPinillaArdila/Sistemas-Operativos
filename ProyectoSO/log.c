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
 * Descripción: " Implementación del controlador de reservas del parque.
 
 *********************************************************************************************/
#include "log.h"
#include <stdio.h>

void log_controlador(const char* mensaje) {
    printf("[CONTROLADOR] %s\n", mensaje);
}

void log_agente(const char* agente, const char* mensaje) {
    printf("[AGENTE %s] %s\n", agente, mensaje);
}