#ifndef COMUN_H
#define COMUN_H

#include <mqueue.h>
#include "claves.h" 
#define SERVER_QUEUE "/cola_servidor_claves"

// para saber la operacion que nos estan pidiendo
typedef enum {
    OP_DESTROY = 0,
    OP_SET,
    OP_GET,
    OP_MODIFY,
    OP_DELETE,
    OP_EXIST
} operacion_t;

// La estructura gigante que viajará por las colas de mensajes
typedef struct {
    operacion_t op;         // define la función a ejecutar
    char q_name[256];       // define el nombre de la funcion a utilizar
    
    // Hdefinimos los espacios necesarios para cada parametro
    char key[256];
    char value1[256];
    int N_value2;
    float V_value2[32];
    struct Paquete value3;
    
    // el servidor mete el código de retorno (0, -1, o -2)
    int resultado;          
} Mensaje;

#endif