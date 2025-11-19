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
 *  Implementación del controlador de reservas del parque.
*              Este programa gestiona las solicitudes de reservas de
*              familias y asigna horas de entrada al parque.
*              Utiliza hilos para manejar el reloj y procesar solicitudes
*              de agentes, y semáforos para sincronizar el acceso a los
*              recursos compartidos.
 * "
 *********************************************************************************************/
#ifndef LOG_H
#define LOG_H

void log_controlador(const char* mensaje);
void log_agente(const char* agente, const char* mensaje);

#endif