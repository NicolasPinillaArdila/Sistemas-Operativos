/*********************************************************************************************
* Pontificia Universidad Javeriana
*
* Autor: Juan Diego Rojas Osorio
*        Juan José Ballesteros
*        Nicolas Pinilla Ardila
* Materia Sistemas Operativos
* Docente: J. Corredor, PhD
* Fecha: 18/11/2025
* Tema: Proyecto Final
*
* Descripción:
*  Este programa implementa la parte del agente en un sistema de reservas de un parque.
*  Su funcion principal es actuar como intermediario entre el controlador y los visitantes del parque,
*  gestionando las solicitudes de reserva y enviando las respuestas al controlador.
**********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "protocolo.h"
#include "pipes.h"
#include "log.h"

/*
* Función principal en la que:
* 1. Se validan los argumentos de línea de comandos
* 2. Se cargan los parámetros ingresados al agente:
*    - Nombre del agente
*    - Archivo con solicitudes de reserva
*    - Pipe por el cual enviará mensajes al controlador
* 3. Se crea un pipe FIFO único para recibir respuestas del controlador,
*    usando el nombre del agente y el PID del proceso.
* 5. Se abre el pipe hacia el controlador y se envía un mensaje de tipo "registro", indicando el nombre del agente y el pipe de respuesta.
* 6. Se espera la respuesta del controlador para confirmar el registro, se espera un maximo de 10 intentos.
* 7. Se imprime la hora inicial del sistema recibida del controlador.
* 8. Se abre el archivo de solicitudes y se procesan línea por línea:
*    - Se lee el nombre de la familia, la hora solicitada y el número de personas.
*    - Se descartan solicitudes extemporáneas (hora < hora actual).
*    - Se envía cada solicitud al controlador mediante el pipe.
*    - Se espera la respuesta inmediata por el pipe de respuesta.
*    - Se imprime el resultado (aprobada, reprogramada o negada).
*    - Se aplica un retardo configurable entre solicitudes (TIEMPO_ESPERA_AGENTE).
* 9. Cuando se procesan todas las solicitudes, se envía un mensaje "fin" al controlador indicando que el agente ha finalizado su trabajo.
* 10. Se cierran los pipes utilizados y se elimina el pipe FIFO de respuesta del agente.
* 11. Se imprime un mensaje final indicando que el agente ha terminado su ejecución.
*/
int main(int argc, char* argv[]) {
    char nombre_agente[50] = "";  //Nombre del agente
    char archivo_solicitudes[100] = "";  //Nombre del archivo que contiene las solicitudes
    char pipe_recibe_nombre[100] = "";  //Pipe por donde se envian los mensajes al controlador
    char pipe_respuesta_nombre[100] = "";  //Pipe por donde se reciben las respuestas del controlador

    //Se verifica que se ingresen los argumentos correctos
    if (argc != 7) {
        printf("Uso: %s -s nombre -a filesolicitud -p pipeRecibe\n", argv[0]);
        exit(1);
    }

    //Se leen los argumentos ingresados por el usuario y se van guardando en las variables correspondientes
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) 
            strcpy(nombre_agente, argv[++i]);
        else if (strcmp(argv[i], "-a") == 0) 
            strcpy(archivo_solicitudes, argv[++i]);
        else if (strcmp(argv[i], "-p") == 0) 
            strcpy(pipe_recibe_nombre, argv[++i]);
    }

    //Se crea el pipe de respuesta del agente usando el nombre del agente y el PID del proceso
    sprintf(pipe_respuesta_nombre, "/tmp/pipe_respuesta_%s_%d", nombre_agente, getpid());
    crear_pipe(pipe_respuesta_nombre);

    log_agente(nombre_agente, "Agente de Reservas iniciado");  //Se anuncia que el agente ha sido iniciado

    //Se abre el pipe para recibir los mensajes del controlador
    int pipe_controlador_fd = abrir_pipe_escritura(pipe_recibe_nombre);
    if (pipe_controlador_fd < 0) {
        printf("Error: No se pudo conectar con el controlador\n");
        exit(1);
    }

    Mensaje registro;  //Se crea el mensaje de registro
    strcpy(registro.tipo, "registro");
    strcpy(registro.agente, nombre_agente);
    strcpy(registro.pipe_respuesta, pipe_respuesta_nombre);

    //Se envia el mensaje de registro
    escribir_mensaje(pipe_controlador_fd, &registro);
    close(pipe_controlador_fd);

    //Se abre el pipe para recibir las respuestas del controlador
    int pipe_respuesta_fd = abrir_pipe_lectura(pipe_respuesta_nombre);
    Mensaje respuesta_registro;

    //Se intenta leer la respuesta del controlador, si no se recibe en 10 intentos se termina el programa, cada intento de un segundo
    int intentos = 0;
    while (intentos < 10) {
        int bytes = leer_mensaje(pipe_respuesta_fd, &respuesta_registro);
        if (bytes > 0) break;  //Si se recibe la respuesta se sale del ciclo
        sleep(1);  //Se espera 1 segundo entre cada intento
        intentos++;
    }

    //Si no se recibe la respuesta en 10 intentos se informa al usuario y se termina el programa
    if (intentos >= 10) {
        printf("Error: No se recibió respuesta del controlador\n");
        exit(1);
    }

    //Si se recibe la respuesta se imprime la hora actual del sistema
    printf("Registro exitoso. Hora actual del sistema: %d\n", respuesta_registro.hora_asignada);


    //Intenta abrir el archivo de solicitudes
    FILE* archivo = fopen(archivo_solicitudes, "r");
    if (!archivo) {
        printf("Error: No se pudo abrir el archivo %s\n", archivo_solicitudes);
        exit(1);
    }

    char linea[TAM_BUFFER];  //Se leen las lineas del archivo de solicitudes   

    //Se lee el archivo linea por linea
    while (fgets(linea, sizeof(linea), archivo)) {

        //Se verifica que el controlador este disponible
        if (access(pipe_recibe_nombre, F_OK) == -1) {
            printf("Controlador no disponible. Terminando agente.\n");
            break;
        }
        //Se lee el nombre de la familia, la hora solicitada y el numero de personas
        char familia[50];
        int hora, personas;

        if (sscanf(linea, "%[^,],%d,%d", familia, &hora, &personas) == 3) {

            //Se verifica que la hora solicitada no sea anterior a la hora actual
            if (hora < respuesta_registro.hora_asignada) {
                printf("Solicitud para familia %s: HORA EXTRANTERÁNEA (%d < %d)\n", 
                       familia, hora, respuesta_registro.hora_asignada);
                continue;
            }


            pipe_controlador_fd = abrir_pipe_escritura(pipe_recibe_nombre);  //Se abre el pipe para enviar los mensajes al controlador

            //Se crea el mensaje de solicitud que sera enviado al controlador
            Mensaje solicitud;
            strcpy(solicitud.tipo, "solicitud");
            strcpy(solicitud.agente, nombre_agente);
            strcpy(solicitud.familia, familia);
            strcpy(solicitud.pipe_respuesta, pipe_respuesta_nombre);
            solicitud.hora_solicitada = hora;
            solicitud.num_personas = personas;

            escribir_mensaje(pipe_controlador_fd, &solicitud);  //Se escribe el mensaje en el pipe
            close(pipe_controlador_fd);  //Cierra el pipe

            //Lee la respuesta del controlador
            Mensaje respuesta;
            memset(&respuesta, 0, sizeof(Mensaje));  // LIMPIAR el buffer

            // ESPERAR activamente la respuesta correcta
            int bytes = 0;
            int intentos = 0;
            while (intentos < 5 && bytes <= 0) {
                bytes = leer_mensaje(pipe_respuesta_fd, &respuesta);
                if (bytes > 0) {
                    // VERIFICAR que la respuesta corresponde a la familia correcta
                    if (strcmp(respuesta.familia, familia) == 0) {
                        break;  // Respuesta correcta
                    } else {
                        // Respuesta para otra familia, seguir esperando
                        bytes = 0;
                    }
                }
                sleep(1);
                intentos++;
            }

            if (bytes > 0 && strcmp(respuesta.familia, familia) == 0) {
                printf("Respuesta para familia %s: %s\n", familia, respuesta.respuesta);
            } else {
                printf("Error: No se recibió respuesta válida para familia %s\n", familia);
            }

            sleep(TIEMPO_ESPERA_AGENTE);   //Espera entre solicitudes
        }
    }

    fclose(archivo);  //Cierra el archivo de solicitudes

    //Se envia un mensaje de fin al controlador
    pipe_controlador_fd = abrir_pipe_escritura(pipe_recibe_nombre);
    Mensaje fin;
    strcpy(fin.tipo, "fin");
    strcpy(fin.agente, nombre_agente);
    escribir_mensaje(pipe_controlador_fd, &fin);
    close(pipe_controlador_fd);


    close(pipe_respuesta_fd);
    unlink(pipe_respuesta_nombre);

    printf("Agente %s termina.\n", nombre_agente);  //Informa que el agente ha terminado su ejecucion
    return 0;
}

