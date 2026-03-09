#include <stdio.h>
#include <string.h>
#include "claves.h"

int main() {
    printf("\n=== PRUEBA 2: CASOS DE ERROR Y VALIDACIONES ===\n\n");
    int res;
    
    // 1. Probar clave demasiado larga (>255)
    printf("1. Probando clave demasiado larga...\n");
    char key_larga[300];
    for (int i = 0; i < 299; i++) key_larga[i] = 'a';
    key_larga[299] = '\0';
    
    float v2[] = {1.0, 2.0};
    struct Paquete p = {1, 2, 3};
    
    res = set_value(key_larga, "test", 2, v2, p);
    printf("   set_value con clave larga (esperado -1): %d\n", res);
    
    // 2. Probar value1 demasiado largo
    printf("\n2. Probando value1 demasiado largo...\n");
    char value1_largo[300];
    for (int i = 0; i < 299; i++) value1_largo[i] = 'b';
    value1_largo[299] = '\0';
    
    res = set_value("clave_normal", value1_largo, 2, v2, p);
    printf("   set_value con value1 largo (esperado -1): %d\n", res);
    
    // 3. Probar N_value2 fuera de rango (menor que 1)
    printf("\n3. Probando N_value2 = 0 (menor que 1)...\n");
    res = set_value("clave_test", "valor", 0, v2, p);
    printf("   set_value con N=0 (esperado -1): %d\n", res);
    
    // 4. Probar N_value2 fuera de rango (mayor que 32)
    printf("\n4. Probando N_value2 = 33 (mayor que 32)...\n");
    float v2_grande[33];
    for (int i = 0; i < 33; i++) v2_grande[i] = i * 1.0;
    res = set_value("clave_test", "valor", 33, v2_grande, p);
    printf("   set_value con N=33 (esperado -1): %d\n", res);
    
    // 5. Insertar una clave válida para pruebas posteriores
    printf("\n5. Insertando clave válida para pruebas...\n");
    float v2_valido[] = {1.5, 2.5, 3.5};
    res = set_value("clave_valida", "valor correcto", 3, v2_valido, p);
    printf("   set_value clave_valida (esperado 0): %d\n", res);
    
    // 6. Probar get_value con clave inexistente
    printf("\n6. Probando get_value con clave inexistente...\n");
    char v1[256];
    int n;
    float v_out[32];
    struct Paquete p_out;
    
    res = get_value("clave_no_existe", v1, &n, v_out, &p_out);
    printf("   get_value clave inexistente (esperado -1): %d\n", res);
    
    // 7. Probar modify_value con clave inexistente
    printf("\n7. Probando modify_value con clave inexistente...\n");
    res = modify_value("clave_no_existe", "nuevo valor", 2, v2, p);
    printf("   modify_value clave inexistente (esperado -1): %d\n", res);
    
    // 8. Probar delete_key con clave inexistente
    printf("\n8. Probando delete_key con clave inexistente...\n");
    res = delete_key("clave_no_existe");
    printf("   delete_key clave inexistente (esperado -1): %d\n", res);
    
    // 9. Probar get_value con punteros NULL
    printf("\n9. Probando get_value con parámetros NULL...\n");
    res = get_value("clave_valida", NULL, &n, v_out, &p_out);
    printf("   get_value con value1 NULL (esperado -1): %d\n", res);
    
    // 10. Probar set_value con parámetros NULL
    printf("\n10. Probando set_value con parámetros NULL...\n");
    res = set_value(NULL, "valor", 2, v2, p);
    printf("   set_value con key NULL (esperado -1): %d\n", res);
    
    // 11. Limpiar
    printf("\n11. Destruyendo sistema...\n");
    destroy();
    
    printf("\n=== FIN PRUEBA 2 ===\n");
    return 0;
}