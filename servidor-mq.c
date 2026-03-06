#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "comun.h"
#include "claves.h"

// esta función ejecuta cada hilo
void *procesar_peticion(void *arg) {
    Mensaje *msg = (Mensaje *)arg;
    Mensaje respuesta;
    
    // copiamos el mensaje que se ha recibido recibido a la respuesta
    memcpy(&respuesta, msg, sizeof(Mensaje));
    
    // evaluamos cual operación es la que nos ha pedido el cliente
    switch(msg->op) {
        case OP_DESTROY:
            respuesta.resultado = destroy();
            break;
        case OP_SET:
            respuesta.resultado = set_value(msg->key, msg->value1, msg->N_value2, msg->V_value2, msg->value3);
            break;
        case OP_GET:
            respuesta.resultado = get_value(msg->key, respuesta.value1, &respuesta.N_value2, respuesta.V_value2, &respuesta.value3);
            break;
        case OP_MODIFY:
            respuesta.resultado = modify_value(msg->key, msg->value1, msg->N_value2, msg->V_value2, msg->value3);
            break;
        case OP_DELETE:
            respuesta.resultado = delete_key(msg->key);
            break;
        case OP_EXIST:
            respuesta.resultado = exist(msg->key);
            break;
        default:
            respuesta.resultado = -1; // si no se conoce la operacion devuelve -1
            break;
    }

    // se envia la respuesta de vuelta por la cola privada del cliente

    mqd_t q_cliente = mq_open(msg->q_name, O_WRONLY);
    if (q_cliente != (mqd_t)-1) {
        mq_send(q_cliente, (const char *)&respuesta, sizeof(Mensaje), 0);
        mq_close(q_cliente);
    } else {
        printf("[Servidor] Error: no se pudo abrir la cola del cliente %s\n", msg->q_name);
    }

    // liberamos la memoria del mensaje que reservamos en el main y cerramos el hilo
    free(msg);
    pthread_exit(NULL);
}

int main() {
    mqd_t q_servidor;
    struct mq_attr attr;

    // configuración de la cola del servidor
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10; // máximo de mensajes en espera
    attr.mq_msgsize = sizeof(Mensaje);
    attr.mq_curmsgs = 0;

    // vaciamos la cola por si no se quedó vacia de alguna operación anterior
    mq_unlink(SERVER_QUEUE); 

    // creamos y abrimos la cola del servidor
    q_servidor = mq_open(SERVER_QUEUE, O_CREAT | O_RDONLY, 0644, &attr);
    if (q_servidor == (mqd_t)-1) {
        perror("[Servidor] Error creando la cola del servidor");
        return -1;
    }

    printf("Servidor de Claves Iniciado\n");
    printf("Escuchando en la cola: %s\n", SERVER_QUEUE);

    while (1) {
        // reservamos memoria para el mensaje entrante 
        Mensaje *msg = (Mensaje *)malloc(sizeof(Mensaje));
        if (msg == NULL) {
            perror("[Servidor] Error de memoria");
            continue;
        }

        // leemos el mensaje. Esperamos aquí hasta que llegue algo
        if (mq_receive(q_servidor, (char *)msg, sizeof(Mensaje), NULL) == -1) {
            perror("[Servidor] Error recibiendo mensaje");
            free(msg);
            continue;
        }

        // creamos un hilo independiente para procesar la petición
        pthread_t hilo;
        pthread_attr_t t_attr;
        pthread_attr_init(&t_attr);
        pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);
        
        pthread_create(&hilo, &t_attr, procesar_peticion, (void *)msg);
        
        pthread_attr_destroy(&t_attr);
    }

    mq_close(q_servidor);
    mq_unlink(SERVER_QUEUE);
    return 0;
}