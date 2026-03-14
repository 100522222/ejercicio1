#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include "claves.h"

#define NUM_CLIENTES 3  // Número de procesos cliente a lanzar

// Función que ejecuta cada cliente - SOLO para versión DISTRIBUIDA
void ejecutar_cliente(int id_cliente) {
    printf("[Cliente %d] Iniciando (PID: %d)\n", id_cliente, getpid());
    
    char key[100];
    float v2[3] = {id_cliente * 1.1, id_cliente * 2.2, id_cliente * 3.3};
    struct Paquete p = {id_cliente, id_cliente * 10, id_cliente * 100};
    char value1[100];
    
    // Cada cliente hace 2 operaciones con claves diferentes
    for (int op = 0; op < 2; op++) {
        sprintf(key, "cliente%d_operacion%d", id_cliente, op);
        sprintf(value1, "valor_cliente%d_op%d", id_cliente, op);
        
        // 1. Insertar
        printf("[Cliente %d] Insertando %s\n", id_cliente, key);
        int res = set_value(key, value1, 3, v2, p);
        if (res == 0) 
            printf("[Cliente %d]   -> OK insertado\n", id_cliente);
        else
            printf("[Cliente %d]   -> ERROR insertando (código %d)\n", id_cliente, res);
        
        usleep(50000);
        
        // 2. Verificar existencia
        printf("[Cliente %d] Verificando %s\n", id_cliente, key);
        int existe = exist(key);
        if (existe == 1)
            printf("[Cliente %d]   -> OK existe\n", id_cliente);
        else
            printf("[Cliente %d]   -> ERROR no existe (código %d)\n", id_cliente, existe);
        
        usleep(50000);
        
        // 3. Obtener valor
        printf("[Cliente %d] Obteniendo %s\n", id_cliente, key);
        char v1_out[256];
        int n_out;
        float v2_out[32];
        struct Paquete p_out;
        
        res = get_value(key, v1_out, &n_out, v2_out, &p_out);
        if (res == 0) {
            printf("[Cliente %d]   -> OK obtenido: %s, N=%d, Paquete(%d,%d,%d)\n", 
                   id_cliente, v1_out, n_out, p_out.x, p_out.y, p_out.z);
        } else {
            printf("[Cliente %d]   -> ERROR obteniendo (código %d)\n", id_cliente, res);
        }
        
        usleep(50000);
        
        // 4. Modificar valor
        printf("[Cliente %d] Modificando %s\n", id_cliente, key);
        sprintf(value1, "modificado_cliente%d_op%d", id_cliente, op);
        p.x += 100;
        
        res = modify_value(key, value1, 2, v2, p);
        if (res == 0)
            printf("[Cliente %d]   -> OK modificado\n", id_cliente);
        else
            printf("[Cliente %d]   -> ERROR modificando (código %d)\n", id_cliente, res);
        
        usleep(50000);
    }
    
    printf("[Cliente %d] Finalizado (PID: %d)\n", id_cliente, getpid());
}

int main() {
    printf("\n=== PRUEBA DE CONCURRENCIA CON MÚLTIPLES CLIENTES (VERSIÓN DISTRIBUIDA) ===\n");
    printf("Lanzando %d procesos cliente simultáneamente\n\n", NUM_CLIENTES);
    
    pid_t pids[NUM_CLIENTES];
    
    // Destruir estado previo SOLO UNA VEZ (el padre)
    printf("[Padre] Limpiando estado inicial...\n");
    destroy();
    
    // Crear procesos cliente
    for (int i = 0; i < NUM_CLIENTES; i++) {
        pids[i] = fork();
        
        if (pids[i] < 0) {
            perror("Error en fork");
            return -1;
        } else if (pids[i] == 0) {
            // Proceso hijo - ejecuta el cliente
            ejecutar_cliente(i);
            exit(0); // El hijo termina aquí
        } else {
            // Proceso padre
            printf("[Padre] Cliente %d lanzado con PID: %d\n", i, pids[i]);
            usleep(100000); // 100ms entre lanzamientos
        }
    }
    
    // El padre espera a que terminen todos los hijos
    printf("\n[Padre] Esperando a que terminen todos los clientes...\n\n");
    
    for (int i = 0; i < NUM_CLIENTES; i++) {
        int status;
        pid_t pid_terminado = wait(&status);
        
        if (pid_terminado > 0) {
            for (int j = 0; j < NUM_CLIENTES; j++) {
                if (pids[j] == pid_terminado) {
                    printf("[Padre] Cliente %d (PID: %d) terminado. ", j, pid_terminado);
                    if (WIFEXITED(status)) {
                        printf("Estado de salida: %d\n", WEXITSTATUS(status));
                    }
                    break;
                }
            }
        }
    }
    
    // Verificación final (el padre comprueba)
    printf("\n=== VERIFICACIÓN FINAL ===\n");
    printf("Comprobando que todas las claves existen...\n");
    
    int total_ok = 0;
    int total_error = 0;
    
    for (int i = 0; i < NUM_CLIENTES; i++) {
        for (int op = 0; op < 2; op++) {
            char key[100];
            sprintf(key, "cliente%d_operacion%d", i, op);
            
            int existe = exist(key);
            if (existe == 1) {
                printf("  Clave %s: OK\n", key);
                total_ok++;
            } else if (existe == 0) {
                printf("  Clave %s: ERROR (no existe)\n", key);
                total_error++;
            } else {
                printf("  Clave %s: ERROR en exist (código %d)\n", key, existe);
                total_error++;
            }
        }
    }
    
    printf("\nResultados finales:\n");
    printf("  Claves existentes correctamente: %d\n", total_ok);
    printf("  Claves perdidas/error: %d\n", total_error);
    
    // Limpiar SOLO AL FINAL (el padre)
    printf("\n[Padre] Destruyendo sistema...\n");
    destroy();
    
    printf("\n=== FIN PRUEBA 3 (VERSIÓN DISTRIBUIDA) ===\n");
    return 0;
}