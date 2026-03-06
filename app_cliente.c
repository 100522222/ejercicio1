#include <stdio.h>
#include <string.h>
#include "claves.h"

int main() {
    printf(" Iniciando pruebas locales\n");

    // Datos de prueba
    float vector_in[3] = {1.5, 2.5, 3.5};
    struct Paquete paq_in = {10, 20, 30};
    
    // 1. Probar inserción
    printf("1. Insertando valor...\n");
    int res = set_value("mi_clave", "Hola Mundo", 3, vector_in, paq_in);
    if (res == 0) printf("   -> Éxito al insertar.\n");
    else printf("   -> ERROR al insertar.\n");

    // 2. Probar existencia
    printf("2. Comprobando si existe 'mi_clave'...\n");
    if (exist("mi_clave") == 1) printf("   -> La clave existe.\n");
    else printf("   -> ERROR: La clave no existe.\n");

    // 3. Probar obtención (get_value)
    char val1[256];
    int n_val2;
    float vector_out[32];
    struct Paquete paq_out;

    printf("3. Obteniendo valores...\n");
    res = get_value("mi_clave", val1, &n_val2, vector_out, &paq_out);
    if (res == 0) {
        printf("   -> Datos obtenidos: %s | N=%d | Paquete(%d,%d,%d)\n", 
               val1, n_val2, paq_out.x, paq_out.y, paq_out.z);
    } else {
        printf("   -> ERROR al obtener datos.\n");
    }

    // 4. Limpiar
    printf("4. Destruyendo sistema...\n");
    destroy();
    
    if (exist("mi_clave") == 0) printf("   -> Sistema limpio. Todo correcto.\n");

    printf(" Fin de las pruebas \n");
    return 0;
}