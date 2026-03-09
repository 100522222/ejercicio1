#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "claves.h"

#define NUM_HILOS 2  // Solo 2 hilos para evitar saturación

typedef struct {
    int id_hilo;
} thread_args_t;

void* pruebas_hilo(void* arg) {
    thread_args_t* args = (thread_args_t*)arg;
    char key[100];
    float v2[2] = {1.1, 2.2};
    struct Paquete p = {args->id_hilo, args->id_hilo*10, args->id_hilo*100};
    
    printf("[Hilo %d] Iniciando\n", args->id_hilo);
    
    // Cada hilo usa su propia clave única
    sprintf(key, "clave_hilo_%d", args->id_hilo);
    
    // 1. Insertar
    int res = set_value(key, "valor_inicial", 2, v2, p);
    printf("[Hilo %d] Insertar %s: %s\n", args->id_hilo, key, 
           res == 0 ? "OK" : "ERROR");
    
    // 2. Verificar
    int existe = exist(key);
    printf("[Hilo %d] Exist %s: %s\n", args->id_hilo, key,
           existe == 1 ? "OK" : "ERROR");
    
    // 3. Obtener
    char v1_out[256];
    int n_out;
    float v2_out[32];
    struct Paquete p_out;
    
    res = get_value(key, v1_out, &n_out, v2_out, &p_out);
    printf("[Hilo %d] Get %s: %s\n", args->id_hilo, key,
           res == 0 ? "OK" : "ERROR");
    
    // 4. Modificar
    res = modify_value(key, "valor_modificado", 1, v2, p);
    printf("[Hilo %d] Modificar %s: %s\n", args->id_hilo, key,
           res == 0 ? "OK" : "ERROR");
    
    printf("[Hilo %d] Finalizado\n", args->id_hilo);
    return NULL;
}

int main() {
    printf("\n=== PRUEBA DE CONCURRENCIA SIMPLIFICADA ===\n");
    printf("Probando con %d hilos (cada uno con clave única)\n\n", NUM_HILOS);
    
    pthread_t hilos[NUM_HILOS];
    thread_args_t args[NUM_HILOS];
    
    // Destruir estado previo
    destroy();
    
    // Crear hilos SECUENCIALMENTE
    for (int i = 0; i < NUM_HILOS; i++) {
        args[i].id_hilo = i;
        pthread_create(&hilos[i], NULL, pruebas_hilo, &args[i]);
        printf("Hilo %d creado\n", i);
        
        // Pequeña pausa entre creaciones para no saturar
        usleep(100000); // 0.1 segundos
    }
    
    // Esperar que todos terminen
    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(hilos[i], NULL);
        printf("Hilo %d terminado\n", i);
    }
    
    // Verificación final
    printf("\n=== VERIFICACIÓN FINAL ===\n");
    for (int i = 0; i < NUM_HILOS; i++) {
        char key[100];
        sprintf(key, "clave_hilo_%d", i);
        
        if (exist(key) == 1) {
            printf("  Clave %s: OK\n", key);
        } else {
            printf("  Clave %s: ERROR (no existe)\n", key);
        }
    }
    
    // Limpiar
    printf("\nDestruyendo sistema...\n");
    destroy();
    
    printf("\n=== FIN PRUEBA ===\n");
    return 0;
}