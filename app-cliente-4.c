#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "claves.h"

// Función para generar strings aleatorios
void generar_string_aleatorio(char *str, int longitud) {
    static const char caracteres[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()";
    for (int i = 0; i < longitud - 1; i++) {
        str[i] = caracteres[rand() % (sizeof(caracteres) - 1)];
    }
    str[longitud - 1] = '\0';
}

int main() {
    printf("\n=== PRUEBA 4: CASOS LÍMITE ===\n\n");
    
    srand(time(NULL));
    int res;
    
    // 1. Probar con el máximo de elementos (N_value2 = 32)
    printf("1. Probando N_value2 = 32 (máximo permitido)\n");
    float v2_max[32];
    for (int i = 0; i < 32; i++) {
        v2_max[i] = i * 0.5f;
    }
    struct Paquete p = {100, 200, 300};
    
    res = set_value("clave_max", "valor_max", 32, v2_max, p);
    printf("   set_value con N=32 (esperado 0): %d\n", res);
    
    // Verificar que se guardó correctamente
    char v1_out[256];
    int n_out;
    float v2_out[32];
    struct Paquete p_out;
    
    res = get_value("clave_max", v1_out, &n_out, v2_out, &p_out);
    if (res == 0) {
        printf("   get_value OK: N=%d, primer float=%f, último float=%f\n", 
               n_out, v2_out[0], v2_out[31]);
    }
    
    // 2. Probar con N_value2 = 1 (mínimo permitido)
    printf("\n2. Probando N_value2 = 1 (mínimo permitido)\n");
    float v2_min[1] = {42.5f};
    
    res = set_value("clave_min", "valor_min", 1, v2_min, p);
    printf("   set_value con N=1 (esperado 0): %d\n", res);
    
    res = get_value("clave_min", v1_out, &n_out, v2_out, &p_out);
    if (res == 0) {
        printf("   get_value OK: N=%d, valor=%f\n", n_out, v2_out[0]);
    }
    
    // 3. Probar con strings de longitud máxima (255)
    printf("\n3. Probando strings de longitud máxima (255 caracteres)\n");
    
    char key_max[256];
    char value1_max[256];
    
    generar_string_aleatorio(key_max, 255);
    generar_string_aleatorio(value1_max, 255);
    
    printf("   Key generada (primeros 50 chars): %.50s...\n", key_max);
    printf("   Value1 generado (primeros 50 chars): %.50s...\n", value1_max);
    
    res = set_value(key_max, value1_max, 2, v2_max, p);
    printf("   set_value con strings de 255 chars (esperado 0): %d\n", res);
    
    // Verificar que se guardó correctamente
    res = get_value(key_max, v1_out, &n_out, v2_out, &p_out);
    if (res == 0) {
        printf("   get_value OK, value1 recuperado (primeros 50 chars): %.50s...\n", v1_out);
    }
    
    // 4. Probar modificar con diferentes N_value2
    printf("\n4. Probando modificación con diferentes N_value2\n");
    
    float v2_mod[5] = {1.1, 2.2, 3.3, 4.4, 5.5};
    res = modify_value("clave_max", "modificado_max", 5, v2_mod, p);
    printf("   modify cambiando N de 32 a 5 (esperado 0): %d\n", res);
    
    res = get_value("clave_max", v1_out, &n_out, v2_out, &p_out);
    if (res == 0) {
        printf("   get_value tras modificar: N=%d, valores: ", n_out);
        for (int i = 0; i < n_out; i++) {
            printf("%.1f ", v2_out[i]);
        }
        printf("\n");
    }
    
    // 5. Probar múltiples claves con diferentes longitudes de key
    printf("\n5. Probando múltiples claves de diferentes longitudes\n");
    
    for (int len = 1; len <= 255; len += 50) {
        char key_var[256];
        generar_string_aleatorio(key_var, len);
        
        char value1_var[256];
        sprintf(value1_var, "valor_para_longitud_%d", len);
        
        printf("   Insertando con key de longitud %d\n", len);
        res = set_value(key_var, value1_var, 2, v2_max, p);
        
        if (res == 0 && exist(key_var) == 1) {
            printf("     -> OK\n");
        } else {
            printf("     -> ERROR\n");
        }
    }
    
    // 6. Probar valores extremos en la estructura Paquete
    printf("\n6. Probando valores extremos en Paquete\n");
    
    struct Paquete p_extremos[] = {
        {0, 0, 0},                    // Ceros
        {2147483647, 2147483647, 2147483647}, // Max int
        {-2147483648, -2147483648, -2147483648}, // Min int
        {42, -100, 1000}              // Mixto
    };
    
    for (int i = 0; i < 4; i++) {
        char key[100];
        sprintf(key, "clave_paquete_%d", i);
        
        res = set_value(key, "test_paquete", 2, v2_max, p_extremos[i]);
        printf("   Insertando Paquete {%d,%d,%d}: ", 
               p_extremos[i].x, p_extremos[i].y, p_extremos[i].z);
        
        if (res == 0) {
            // Recuperar y verificar
            res = get_value(key, v1_out, &n_out, v2_out, &p_out);
            if (res == 0 && 
                p_out.x == p_extremos[i].x && 
                p_out.y == p_extremos[i].y && 
                p_out.z == p_extremos[i].z) {
                printf("OK\n");
            } else {
                printf("ERROR en verificación\n");
            }
        } else {
            printf("ERROR en inserción\n");
        }
    }
    
    // 7. Probar operaciones combinadas
    printf("\n7. Probando operaciones combinadas en cadena\n");
    
    // Insertar
    res = set_value("cadena_test", "inicial", 3, v2_max, p);
    printf("   set_value inicial (esperado 0): %d\n", res);
    
    // Exist
    printf("   exist (esperado 1): %d\n", exist("cadena_test"));
    
    // Get
    res = get_value("cadena_test", v1_out, &n_out, v2_out, &p_out);
    printf("   get_value (esperado 0): %d, value1='%s'\n", res, v1_out);
    
    // Modify
    res = modify_value("cadena_test", "modificado", 1, v2_min, p);
    printf("   modify_value (esperado 0): %d\n", res);
    
    // Get tras modificar
    res = get_value("cadena_test", v1_out, &n_out, v2_out, &p_out);
    printf("   get_value tras modificar: value1='%s', N=%d\n", v1_out, n_out);
    
    // Delete
    res = delete_key("cadena_test");
    printf("   delete_key (esperado 0): %d\n", res);
    
    // Exist tras borrar
    printf("   exist tras borrar (esperado 0): %d\n", exist("cadena_test"));
    
    // 8. Limpiar todo
    printf("\n8. Destruyendo sistema...\n");
    destroy();
    
    // Verificar que todo está limpio
    printf("   Verificando sistema limpio...\n");
    if (exist("clave_max") == 0 && exist("clave_min") == 0) {
        printf("   OK: Sistema correctamente limpiado\n");
    } else {
        printf("   ERROR: Quedan claves residuales\n");
    }
    
    printf("\n=== FIN PRUEBA 4 ===\n");
    return 0;
}