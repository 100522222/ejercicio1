#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "claves.h"
#include "comun.h"


// envia la petición al servidor y espera la respuesta

int enviar_y_recibir(Mensaje *msg) {
    char q_cliente_name[256];
    
    // 1. crea un nombre único para la cola de este cliente usando su PID
    snprintf(q_cliente_name, sizeof(q_cliente_name), "/cola_cliente_%d", getpid());
    strncpy(msg->q_name, q_cliente_name, 255);

    // 2. configura y crea la cola privada del cliente
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(Mensaje);
    attr.mq_curmsgs = 0;

    mqd_t q_cliente = mq_open(q_cliente_name, O_CREAT | O_RDONLY, 0644, &attr);
    if (q_cliente == (mqd_t)-1) {
        perror("[Proxy] Error creando cola de cliente");
        return -2; // -2 si hay un error de comunicación
    }

    // 3. abre la cola del servidor para enviarle el mensaje
    mqd_t q_servidor = mq_open(SERVER_QUEUE, O_WRONLY);
    if (q_servidor == (mqd_t)-1) {
        perror("[Proxy] Error abriendo cola del servidor");
        mq_close(q_cliente);
        mq_unlink(q_cliente_name);
        return -2;
    }

    // 4. envia el mensaje al servidor
    if (mq_send(q_servidor, (const char *)msg, sizeof(Mensaje), 0) == -1) {
        perror("[Proxy] Error enviando mensaje al servidor");
        mq_close(q_servidor);
        mq_close(q_cliente);
        mq_unlink(q_cliente_name);
        return -2;
    }

    // 5. espera la respuesta en la cola privada
    if (mq_receive(q_cliente, (char *)msg, sizeof(Mensaje), NULL) == -1) {
        perror("[Proxy] Error recibiendo respuesta");
        mq_close(q_servidor);
        mq_close(q_cliente);
        mq_unlink(q_cliente_name);
        return -2;
    }

    // 6. limpia se cierra y borra usado
    mq_close(q_servidor);
    mq_close(q_cliente);
    mq_unlink(q_cliente_name);

    // devolve el resultado 
    return msg->resultado;
}


// Implementación de la API


int destroy(void) {
    Mensaje msg;
    msg.op = OP_DESTROY;
    return enviar_y_recibir(&msg);
}

int set_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3) {
    if (key == NULL || value1 == NULL || V_value2 == NULL) return -1;
    
    Mensaje msg;
    msg.op = OP_SET;
    strncpy(msg.key, key, 255);
    strncpy(msg.value1, value1, 255);
    msg.N_value2 = N_value2;
    for (int i = 0; i < N_value2; i++) {
        msg.V_value2[i] = V_value2[i];
    }
    msg.value3 = value3;
    
    return enviar_y_recibir(&msg);
}

int get_value(char *key, char *value1, int *N_value2, float *V_value2, struct Paquete *value3) {
    if (key == NULL || value1 == NULL || V_value2 == NULL || value3 == NULL) return -1;

    Mensaje msg;
    msg.op = OP_GET;
    strncpy(msg.key, key, 255);
    
    int res = enviar_y_recibir(&msg);
    
    // si el servidor encontró el dato (res == 0), lo extraemos del mensaje y lo devolvemos al usuario
    if (res == 0) {
        strncpy(value1, msg.value1, 255);
        *N_value2 = msg.N_value2;
        for (int i = 0; i < msg.N_value2; i++) {
            V_value2[i] = msg.V_value2[i];
        }
        *value3 = msg.value3;
    }
    return res;
}

int modify_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3) {
    if (key == NULL || value1 == NULL || V_value2 == NULL) return -1;

    Mensaje msg;
    msg.op = OP_MODIFY;
    strncpy(msg.key, key, 255);
    strncpy(msg.value1, value1, 255);
    msg.N_value2 = N_value2;
    for (int i = 0; i < N_value2; i++) {
        msg.V_value2[i] = V_value2[i];
    }
    msg.value3 = value3;

    return enviar_y_recibir(&msg);
}

int delete_key(char *key) {
    if (key == NULL) return -1;
    Mensaje msg;
    msg.op = OP_DELETE;
    strncpy(msg.key, key, 255);
    return enviar_y_recibir(&msg);
}

int exist(char *key) {
    if (key == NULL) return -1;
    Mensaje msg;
    msg.op = OP_EXIST;
    strncpy(msg.key, key, 255);
    return enviar_y_recibir(&msg);
}
