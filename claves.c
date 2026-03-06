#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "claves.h"

typedef struct Nodo {
    char key[256];
    char value1[256];
    int N_value2;
    float V_value2[32];
    struct Paquete value3;
    struct Nodo *siguiente;
} Nodo;

static Nodo *cabeza = NULL;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


int destroy(void) {
    pthread_mutex_lock(&mutex);
    Nodo *actual = cabeza;
    while (actual != NULL) {
        Nodo *sig = actual->siguiente;
        free(actual);
        actual = sig;
    }
    cabeza = NULL;
    pthread_mutex_unlock(&mutex);
    return 0;
}

int exist(char *key) {
    if (key == NULL) return -1;

    pthread_mutex_lock(&mutex);
    Nodo *actual = cabeza;
    while (actual != NULL) {
        if (strcmp(actual->key, key) == 0) {
            pthread_mutex_unlock(&mutex);
            return 1;
        }
        actual = actual->siguiente;
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

int set_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3) {
    if (key == NULL || value1 == NULL || V_value2 == NULL) return -1;
    if (N_value2 < 1 || N_value2 > 32) return -1;

    pthread_mutex_lock(&mutex);
    
    // Comprobar duplicado
    Nodo *actual = cabeza;
    while (actual != NULL) {
        if (strcmp(actual->key, key) == 0) {
            pthread_mutex_unlock(&mutex);
            return -1; 
        }
        actual = actual->siguiente;
    }

    Nodo *nuevo = (Nodo *)malloc(sizeof(Nodo));
    if (nuevo == NULL) {
        pthread_mutex_unlock(&mutex);
        return -1;
    }

    strncpy(nuevo->key, key, 255);
    nuevo->key[255] = '\0';
    strncpy(nuevo->value1, value1, 255);
    nuevo->value1[255] = '\0';
    nuevo->N_value2 = N_value2;
    for (int i = 0; i < N_value2; i++) nuevo->V_value2[i] = V_value2[i];
    nuevo->value3 = value3;

    nuevo->siguiente = cabeza;
    cabeza = nuevo;

    pthread_mutex_unlock(&mutex);
    return 0;
}

int get_value(char *key, char *value1, int *N_value2, float *V_value2, struct Paquete *value3) {
    if (key == NULL) return -1;

    pthread_mutex_lock(&mutex);
    Nodo *actual = cabeza;
    while (actual != NULL) {
        if (strcmp(actual->key, key) == 0) {
            strncpy(value1, actual->value1, 255);
            value1[255] = '\0';
            *N_value2 = actual->N_value2;
            for (int i = 0; i < actual->N_value2; i++) V_value2[i] = actual->V_value2[i];
            *value3 = actual->value3;

            pthread_mutex_unlock(&mutex);
            return 0;
        }
        actual = actual->siguiente;
    }
    pthread_mutex_unlock(&mutex);
    return -1;
}

int modify_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3) {
    if (N_value2 < 1 || N_value2 > 32) return -1;

    pthread_mutex_lock(&mutex);

    Nodo *actual = cabeza;
    while (actual != NULL) {
        if (strcmp(actual->key, key) == 0) {
            strncpy(actual->value1, value1, 255);
            actual->value1[255] = '\0';

            actual->N_value2 = N_value2;
            for (int i = 0; i < N_value2; i++) {
                actual->V_value2[i] = V_value2[i];
            }
            actual->value3 = value3;

            pthread_mutex_unlock(&mutex);
            return 0;
        }
        actual = actual->siguiente;
    }

    pthread_mutex_unlock(&mutex);
    return -1;
}

int delete_key(char *key) {
    pthread_mutex_lock(&mutex);

    Nodo *actual = cabeza;
    Nodo *anterior = NULL;

    while (actual != NULL) {
        if (strcmp(actual->key, key) == 0) {
            if (anterior == NULL) {
                cabeza = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }
            free(actual);
            pthread_mutex_unlock(&mutex);
            return 0;
        }
        anterior = actual;
        actual = actual->siguiente;
    }

    pthread_mutex_unlock(&mutex);
    return -1;
}