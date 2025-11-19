/*********************************************************************************************
* Pontificia Universidad Javeriana
*
* Autor: Juan Diego Rojas Osorio
*        Juan José Ballesteros
*        Nicolas Pinilla Ardila
* Materia: Sistemas Operativos
* Docente: J. Corredor, PhD
* Fecha: 18/11/2025
* Tema: Proyecto Final - Sistema de Reservas de Parque
*
* Descripción:
*  Implementación del controlador de reservas del parque Berlín.
*  Este programa actúa como servidor central que gestiona las solicitudes
*  de reservas de familias y asigna horas de entrada al parque mediante
*  un sistema de aforo controlado. Utiliza hilos POSIX para manejar
*  concurrentemente el reloj de simulación y las solicitudes de agentes,
*  y mecanismos de sincronización (mutex) para garantizar la consistencia
*  de los datos compartidos. El sistema procesa reservas por bloques de
*  2 horas dentro del horario operativo (7:00 - 19:00) y proporciona
*  reportes detallados de ocupación al finalizar la simulación.
*********************************************************************************************/

#include "log.h"
#include "pipes.h"
#include "protocolo.h"
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

// Variables globales
EstadoParque parque; // Estructura que almacena el estado del parque, en este
                     // caso las personas y familias
int hora_actual;              // Hora actual
int hora_fin;                 // Hora final
int segundos_por_hora;        // Segundos por hora
int aforo_maximo;             // Aforo maximo del parque
char pipe_recibe_nombre[100]; // Nombre del pipe por donde el servidor recibe
                              // los mensajes
int pipe_recibe_fd; // Descriptor del pipe
int ejecutando =
    1; // Variable de control usada para saber si el servidor esta ejecutandose

// Sincronizacion (Mutex y condiciones para controlar el acceso al parque)
pthread_mutex_t mutex_parque = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_hora = PTHREAD_COND_INITIALIZER;

// Funciones (declaraciones)
//void manejar_Senal(int sig);
void *hilo_reloj(void *arg);
void *hilo_agentes(void *arg);
void procesar_cambio_hora();
int procesar_solicitud(Mensaje *solicitud, Mensaje *respuesta);
void generar_reporte_final();

// Funcion encargada de inicializar el parque
void inicializar_parque(EstadoParque *p) {

  strcpy(p->nombre, "Parque Berlin"); // Nombre del parque

  // Se inicializan los vectores de personas y familias por hora, estos se
  // inicializan en 0
  for (int i = 0; i <= MAX_HORA; i++) {
    p->personas_por_hora[i] = 0;
    p->familias_por_hora[i] = 0;
  }

  // Se inicializa la lista de solicitudes, estas se inicializan en 0
  p->num_solicitudes = 0;
  for (int i = 0; i < MAX_FAMILIAS; i++) {
    strcpy(p->solicitudes[i].nombre_familia, "");
    strcpy(p->solicitudes[i].agente, "");
    p->solicitudes[i].hora_solicitada = 0;
    p->solicitudes[i].hora_asignada = -1;
    p->solicitudes[i].num_personas = 0;
    p->solicitudes[i].estado = 0;
  }
}

/*
 * Función principal en la que:
 * 1. Se validan los argumentos ingresados por línea de comandos
 *
 * 2. Se almacenan los parámetros de simulación:
 *    - Hora inicial y final
 *    - Duración en segundos de cada hora simulada
 *    - Aforo máximo permitido por hora
 *    - Nombre del pipe desde el cual se reciben mensajes
 * 3. Se valida que las horas ingresadas se encuentren dentro del rango
 * permitido y que la hora inicial sea menor a la hora final.
 * 4. Se inicializa el parque (estructuras de control, contadores por hora,
 * solicitudes y familias registradas).
 * 5. Se crea el pipe FIFO para recibir mensajes desde los agentes y se abre en
 * modo lectura para recibir mensajes.
 * 7. Se crean dos hilos:
 *    - Hilo reloj: avanza la hora, actualiza salidas de familias y controla el
 * fin del día.
 *    - Hilo agentes: atiende solicitudes que llegan por el pipe, como lo son
 * los registros y las reservas de familias enviadas por los agentes.
 * 8. Se espera a que ambos hilos finalicen su ejecución antes de continuar.
 * 9. Se cierran y eliminan los pipes creados.
 */
int main(int argc, char *argv[]) {

  // Se verifica que se ingresen los argumentos correctos
  if (argc != 11) {
    printf("Uso: %s -i horaIni -f horaFin -s segHoras -t total -p pipeRecibe\n",
           argv[0]);
    exit(1);
  }

  // Se leen los argumentos ingresados por el usuario y se van guardando en las
  // variables correspondientes
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-i") == 0)
      hora_actual = atoi(argv[++i]);
    else if (strcmp(argv[i], "-f") == 0)
      hora_fin = atoi(argv[++i]);
    else if (strcmp(argv[i], "-s") == 0)
      segundos_por_hora = atoi(argv[++i]);
    else if (strcmp(argv[i], "-t") == 0)
      aforo_maximo = atoi(argv[++i]);
    else if (strcmp(argv[i], "-p") == 0)
      strcpy(pipe_recibe_nombre, argv[++i]);
  }

  // Se valida que las horas ingresadas sean validas, es decir que esten en el
  // rango
  if (hora_actual < MIN_HORA || hora_actual > MAX_HORA || hora_fin < MIN_HORA ||
      hora_fin > MAX_HORA || hora_actual >= hora_fin) {
    printf("Error: Horas inválidas. Deben estar entre %d y %d, y horaini < "
           "horafin\n",
           MIN_HORA, MAX_HORA);
    exit(1);
  }

    // UNA inicialización
    inicializar_parque(&parque);

    // Se crea y abre el pipe que recibira los mensajes de los agentes
    crear_pipe(pipe_recibe_nombre);
    pipe_recibe_fd = abrir_pipe_lectura(pipe_recibe_nombre);

    // Se anuncia que el controlador ha sido iniciado, informando la hora inicial,
    // la hora final, los segundos por hora y el aforo maximo
    log_controlador("Controlador de Reservas iniciado");
    printf("Hora inicial: %d, Hora final: %d\n", hora_actual, hora_fin);
    printf("Segundos por hora: %d, Aforo máximo: %d\n", segundos_por_hora, aforo_maximo);
    printf("Esperando solicitudes de agentes...\n");

    // Configurar la señal ANTES de crear los hilos
    //signal(SIGALRM, manejar_Senal);

    // Creacion de hilos para el reloj y los agentes
    pthread_t tid_reloj, tid_agentes;
    pthread_create(&tid_reloj, NULL, hilo_reloj, NULL);
    pthread_create(&tid_agentes, NULL, hilo_agentes, NULL);

    // Se espera a que los hilos terminen
    pthread_join(tid_reloj, NULL);
    pthread_join(tid_agentes, NULL);

    // Se cierra el pipe para no recibir mas mensajes y se elimina el archivo del pipe
    close(pipe_recibe_fd);
    unlink(pipe_recibe_nombre);

    return 0;
}

// Funcion encargada de manejar la senal de alarma
/*void manejar_Senal(int sig) {
  if (sig == SIGALRM) {
    pthread_mutex_lock(&mutex_parque);
    procesar_cambio_hora();
    pthread_mutex_unlock(&mutex_parque);
  }
}*/

// Funcion para el manejo del hilo del reloj
void *hilo_reloj(void *arg) {
    // MOSTRAR HORA INICIAL INMEDIATAMENTE
    printf("\n=== HORA ACTUAL: %d ===\n", hora_actual);
    printf("Personas en el parque a las %d: %d\n", hora_actual, parque.personas_por_hora[hora_actual]);

    while (ejecutando && hora_actual <= hora_fin) {
        sleep(segundos_por_hora); // Simula el paso del tiempo, en este caso 1 hora

        pthread_mutex_lock(&mutex_parque);
        hora_actual++; // Avanza la hora actual

        // Verificar que no excedamos la hora final
        if (hora_actual > hora_fin) {
            pthread_mutex_unlock(&mutex_parque);

            // Esperar a que todos los agentes terminen
            printf("Día terminado. Esperando que agentes finalicen...\n");
            sleep(10); // Esperar 10 segundos para que los agentes terminen

            // Generar el reporte final
            ejecutando = 0;
            generar_reporte_final();
            break;
        }

        printf("\n=== HORA ACTUAL: %d ===\n", hora_actual);

        // Se revisa las familias cuyo turno ya termino, es decir que ya estuvieron
        // dos horas
        int personas_salen = 0;
        for (int i = 0; i < parque.num_solicitudes; i++) {
            if ((parque.solicitudes[i].estado == 1 || parque.solicitudes[i].estado == 2)  &&
                parque.solicitudes[i].hora_asignada + 2 == hora_actual) {
                personas_salen += parque.solicitudes[i].num_personas;
                printf("Familia %s sale del parque (%d personas)\n",
                       parque.solicitudes[i].nombre_familia,
                       parque.solicitudes[i].num_personas);
            }
        }

        // El aforo ya está calculado correctamente por las sumas en
        // procesar_solicitud
        if (hora_actual <= MAX_HORA) {
            printf("Personas en el parque a las %d: %d\n", hora_actual,
                   parque.personas_por_hora[hora_actual]);
        }

        pthread_mutex_unlock(&mutex_parque);

        // Programar la siguiente alarma solo si no hemos llegado al final
        /*if (hora_actual < hora_fin) {
            alarm(segundos_por_hora);
        }*/
    }
    return NULL;
}
// Funcion para el manejo del hilo de los agentes, esta lee las solicitudes de
// los agentes
void *hilo_agentes(void *arg) {
  Mensaje mensaje;

  while (ejecutando) {
    int bytes_leidos = leer_mensaje(pipe_recibe_fd, &mensaje);

    if (bytes_leidos > 0) {

      // Se realiza el registro del agente
      if (strcmp(mensaje.tipo, "registro") == 0) {
        printf("Agente registrado: %s\n", mensaje.agente);

        Mensaje respuesta;
        strcpy(respuesta.tipo, "respuesta");
        strcpy(respuesta.agente, mensaje.agente);
        respuesta.hora_asignada = hora_actual;
        strcpy(respuesta.respuesta,"Registro exitoso"); // Se informa que el registro fue exitoso

        int pipe_respuesta_fd = abrir_pipe_escritura(mensaje.pipe_respuesta);
        escribir_mensaje(pipe_respuesta_fd, &respuesta);
        close(pipe_respuesta_fd);

// Se procesa la solicitud de reserva que envia un agente
} else if (strcmp(mensaje.tipo, "solicitud") == 0) {
    printf("Solicitud recibida - Agente: %s, Familia: %s, Hora: %d, Personas: %d\n",
           mensaje.agente, mensaje.familia, mensaje.hora_solicitada, mensaje.num_personas);

    pthread_mutex_lock(&mutex_parque);

    Mensaje respuesta;
    int resultado = procesar_solicitud(&mensaje, &respuesta);

    // Usar la variable resultado para eliminar el warning
    printf("Resultado del procesamiento para familia %s: %d\n", mensaje.familia, resultado);
    if (resultado == 2) {
    printf(">> Reserva de %s reprogramada para la hora %d\n",
           mensaje.familia, respuesta.hora_asignada);
}

    int pipe_respuesta_fd = abrir_pipe_escritura(mensaje.pipe_respuesta);
    escribir_mensaje(pipe_respuesta_fd, &respuesta);
    close(pipe_respuesta_fd);

    //Recalcular y mostrar aforo actual DESPUÉS de procesar todo
    int personas_actuales = 0;
    for (int i = 0; i < parque.num_solicitudes; i++) {
        if (parque.solicitudes[i].estado == 1 && 
            hora_actual >= parque.solicitudes[i].hora_asignada &&
            hora_actual < parque.solicitudes[i].hora_asignada + 2) {
            personas_actuales += parque.solicitudes[i].num_personas;
        }
    }
    parque.personas_por_hora[hora_actual] = personas_actuales;
    printf("Personas en el parque a las %d: %d\n", hora_actual, personas_actuales);

    pthread_mutex_unlock(&mutex_parque);
        // Se procesa el mensaje de fin de un agente
      } else if (strcmp(mensaje.tipo, "fin") == 0) {
        printf("Agente %s terminó\n", mensaje.agente);
      }
    }
  }
  return NULL;
}

/*Funcion encargada de procesar las solicitudes de los agentes, esta devuelve:
1. Si la solicitud fue aprobada
2. Si la solicitud fue reprogramada
3. Si la solicitud fue rechazada
*/
int procesar_solicitud(Mensaje *solicitud, Mensaje *respuesta) {
  // Almacena la informacion de la solicitud
  strcpy(respuesta->tipo, "respuesta");
  strcpy(respuesta->agente, solicitud->agente);
  strcpy(respuesta->familia, solicitud->familia);
  respuesta->num_personas = solicitud->num_personas;
  respuesta->hora_solicitada = solicitud->hora_solicitada;

  // Se valida que el numero de personas no exceda el aforo maximo
  if (solicitud->num_personas > aforo_maximo) {
      strcpy(respuesta->respuesta, "Reserva negada: número de personas excede el aforo máximo");
      respuesta->hora_asignada = -1;

      //Guardar la solicitud negada
      if (parque.num_solicitudes < MAX_FAMILIAS) {
          Solicitud *s = &parque.solicitudes[parque.num_solicitudes++];
          strcpy(s->nombre_familia, solicitud->familia);
          strcpy(s->agente, solicitud->agente);
          s->hora_solicitada = solicitud->hora_solicitada;
          s->hora_asignada = -1;
          s->num_personas = solicitud->num_personas;
          s->estado = 3;  // Solicitud negada
      }

      return 3;
  }

  //Se valida que la reserva NO termine después del cierre
  // La reserva dura 2 horas, por lo que debe terminar a más tardar a las 19:00
  if (solicitud->hora_solicitada > hora_fin || solicitud->hora_solicitada + 1 >= MAX_HORA) {
      strcpy(respuesta->respuesta, "Reserva negada: el parque cierra a las 19:00");
      respuesta->hora_asignada = -1;

      // Guardar la solicitud negada
      if (parque.num_solicitudes < MAX_FAMILIAS) {
          Solicitud *s = &parque.solicitudes[parque.num_solicitudes++];
          strcpy(s->nombre_familia, solicitud->familia);
          strcpy(s->agente, solicitud->agente);
          s->hora_solicitada = solicitud->hora_solicitada;
          s->hora_asignada = -1;
          s->num_personas = solicitud->num_personas;
          s->estado = 3;  // Solicitud negada
      }

      return 3;
  }
// Se valida que la hora solicitada no sea anterior a la hora actual
if (solicitud->hora_solicitada < hora_actual) {
    strcpy(respuesta->respuesta, "Reserva negada por extemporánea");
    respuesta->hora_asignada = -1;

    // Se reprograma la solicitud
    for (int h = hora_actual; h <= hora_fin - 1; h++) {
        if (parque.personas_por_hora[h] + solicitud->num_personas <= aforo_maximo && 
            parque.personas_por_hora[h + 1] + solicitud->num_personas <= aforo_maximo) {
            respuesta->hora_asignada = h;
            sprintf(respuesta->respuesta, "Reserva reprogramada para la hora %d", h);

            // Sumar a AMBAS horas de la estadía
            parque.personas_por_hora[h] += solicitud->num_personas;
            parque.personas_por_hora[h + 1] += solicitud->num_personas;

            // Registrar la solicitud
            if (parque.num_solicitudes < MAX_FAMILIAS) {
                Solicitud *s = &parque.solicitudes[parque.num_solicitudes++];
                strcpy(s->nombre_familia, solicitud->familia);
                strcpy(s->agente, solicitud->agente);
                s->hora_solicitada = solicitud->hora_solicitada;
                s->hora_asignada = h;
                s->num_personas = solicitud->num_personas;
                s->estado = 2; // Solicitud reprogramada
            }
            return 2;
        }
    }
    strcpy(respuesta->respuesta, "Reserva negada, debe volver otro día");
    return 3;
}
    //Verificar que la hora solicitada permita una reserva de 2 horas
    // (es decir, que termine a más tardar a las 19:00)
    if (solicitud->hora_solicitada + 1 < MAX_HORA &&  solicitud->hora_solicitada <= hora_fin && parque.personas_por_hora[solicitud->hora_solicitada] + solicitud->num_personas <= aforo_maximo &&
        parque.personas_por_hora[solicitud->hora_solicitada + 1] + solicitud->num_personas <= aforo_maximo) {

      if (solicitud->hora_solicitada + 1 >= MAX_HORA) {
        strcpy(respuesta->respuesta, "Reserva negada: el parque cierra a las 19:00");
        Solicitud *s = &parque.solicitudes[parque.num_solicitudes++];
        strcpy(s->nombre_familia, solicitud->familia);
        strcpy(s->agente, solicitud->agente);
        s->hora_solicitada = solicitud->hora_solicitada;
        s->hora_asignada = -1;
        s->num_personas = solicitud->num_personas;
        s->estado = 3;  // Solicitud negada
        return 3;
      }
        // Se aprueba la hora solicitada si esta disponible
        respuesta->hora_asignada = solicitud->hora_solicitada;
        strcpy(respuesta->respuesta, "Reserva OK");

        // Sumar a AMBAS horas de la estadía
        parque.personas_por_hora[solicitud->hora_solicitada] += solicitud->num_personas;
        parque.personas_por_hora[solicitud->hora_solicitada + 1] += solicitud->num_personas;

        // Se realiza el registro de la solicitud
        if (parque.num_solicitudes < MAX_FAMILIAS) {
          Solicitud *s = &parque.solicitudes[parque.num_solicitudes++];
          strcpy(s->nombre_familia, solicitud->familia);
          strcpy(s->agente, solicitud->agente);
          s->hora_solicitada = solicitud->hora_solicitada;
          s->hora_asignada = solicitud->hora_solicitada;
          s->num_personas = solicitud->num_personas;
          s->estado = 1; // Solicitud aprobada
        }

        return 1;

    // Se realiza la reprogramacion de la solicitud
  } else {

    // Se reprograma la solicitud para otra hora disponible
    for (int h = solicitud->hora_solicitada; h <= hora_fin - 2; h++) {
      if (parque.personas_por_hora[h] + solicitud->num_personas <=
              aforo_maximo &&
          parque.personas_por_hora[h + 1] + solicitud->num_personas <=
              aforo_maximo) {
        respuesta->hora_asignada = h;
        sprintf(respuesta->respuesta,
                "Reserva garantizada para otras horas: %d",
                h); // Se reprograma la solicitud

        // Sumar a AMBAS horas de la estadía
        parque.personas_por_hora[h] += solicitud->num_personas;
        parque.personas_por_hora[h + 1] += solicitud->num_personas;

        // Se realiza el registro de la solicitud
        if (parque.num_solicitudes < MAX_FAMILIAS) {
          Solicitud *s = &parque.solicitudes[parque.num_solicitudes++];
          strcpy(s->nombre_familia, solicitud->familia);
          strcpy(s->agente, solicitud->agente);
          s->hora_solicitada = solicitud->hora_solicitada;
          s->hora_asignada = h;
          s->num_personas = solicitud->num_personas;
          s->estado = 2; // Solicitud reprogramada
        }

        return 2;
      }
    }

    // Si no se puede reprogramar la solicitud se rechaza
    strcpy(respuesta->respuesta, "Reserva negada, debe volver otro día");
    respuesta->hora_asignada = -1;

    //Guardar la solicitud negada
    if (parque.num_solicitudes < MAX_FAMILIAS) {
        Solicitud *s = &parque.solicitudes[parque.num_solicitudes++];
        strcpy(s->nombre_familia, solicitud->familia);
        strcpy(s->agente, solicitud->agente);
        s->hora_solicitada = solicitud->hora_solicitada;
        s->hora_asignada = -1;
        s->num_personas = solicitud->num_personas;
        s->estado = 3;  // Solicitud negada
    }

    return 3;  // Solicitud rechazada
  }
}

// Funcion para generar el reporte final del parque
void generar_reporte_final() {
  printf("\n=== REPORTE FINAL ===\n");

  // Declaracion de variables para almacenar los datos del reporte
  int max_personas = 0, min_personas = aforo_maximo;
  int horas_pico[MAX_HORA + 1] = {0};
  int horas_bajas[MAX_HORA + 1] = {0};
  int num_horas_pico = 0, num_horas_bajas = 0;

  int solicitudes_aprobadas = 0, solicitudes_reprogramadas = 0,
      solicitudes_negadas = 0;

  // Se determinan las horas maximas y minimas de personas en el parque
  for (int i = MIN_HORA; i <= MAX_HORA && i <= hora_fin; i++) {
    if (parque.personas_por_hora[i] > max_personas) {
      max_personas = parque.personas_por_hora[i];
    }
    if (parque.personas_por_hora[i] < min_personas) {
      min_personas = parque.personas_por_hora[i];
    }
  }

  // Se realiza el calculo de las horas pico y bajas basandose en el numero de
  // personas en el parque en esas horas
  for (int i = MIN_HORA; i <= MAX_HORA && i <= hora_fin; i++) {
    if (parque.personas_por_hora[i] == max_personas) {
      horas_pico[num_horas_pico++] = i;
    }
    if (parque.personas_por_hora[i] == min_personas) {
      horas_bajas[num_horas_bajas++] = i;
    }
  }

  // Se realiza el calculo de las solicitudes aprobadas, reprogramadas y negadas
  for (int i = 0; i < parque.num_solicitudes; i++) {
    if (parque.solicitudes[i].estado == 1)
      solicitudes_aprobadas++;
    else if (parque.solicitudes[i].estado == 2)
      solicitudes_reprogramadas++;
    else if (parque.solicitudes[i].estado == 3)
      solicitudes_negadas++;
  }

  // Se imprimen las horas pico y horas bajas
  printf("Horas pico (%d personas): ", max_personas);
  for (int i = 0; i < num_horas_pico; i++) {
    printf("%d ", horas_pico[i]);
  }
  printf("\n");

  printf("Horas bajas (%d personas): ", min_personas);
  for (int i = 0; i < num_horas_bajas; i++) {
    printf("%d ", horas_bajas[i]);
  }
  printf("\n");

  // Se imprime el total de solicitudes incluyendo: las solicitudes aprobadas,
  // reprogramadas y negadas
  printf("Solicitudes aprobadas: %d\n", solicitudes_aprobadas);
  printf("Solicitudes reprogramadas: %d\n", solicitudes_reprogramadas);
  printf("Solicitudes negadas: %d\n", solicitudes_negadas);
  printf("Total solicitudes procesadas: %d\n", parque.num_solicitudes);
}

// Funcion para procesar el cambio de hora
void procesar_cambio_hora() {

  printf("Procesando cambio de hora a %d\n", hora_actual);
}