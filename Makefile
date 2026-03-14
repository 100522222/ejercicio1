CC = gcc
CFLAGS = -Wall -g -pthread
LDFLAGS = -L. -Wl,-rpath=./ 

# Todos los objetivos a construir
all: libclaves.so cliente_local servidor libproxyclaves.so cliente_distribuido \
     cliente_local_test2 cliente_local_test4 \
     cliente_distribuido_test2 cliente_distribuido_test3 cliente_distribuido_test4

# ----------------------------------------------------
# FASE A: Versión Local Monolítica
# ----------------------------------------------------

# Biblioteca dinámica para versión local
libclaves.so: claves.c claves.h
	$(CC) $(CFLAGS) -fPIC -shared -o libclaves.so claves.c

# Cliente local básico (app-cliente-1.c original)
cliente_local: app-cliente-1.c libclaves.so
	$(CC) $(CFLAGS) -o cliente_local app-cliente-1.c $(LDFLAGS) -lclaves

# Clientes de prueba adicionales para versión local
cliente_local_test2: app-cliente-2.c libclaves.so
	$(CC) $(CFLAGS) -o cliente_local_test2 app-cliente-2.c $(LDFLAGS) -lclaves

cliente_local_test4: app-cliente-4.c libclaves.so
	$(CC) $(CFLAGS) -o cliente_local_test4 app-cliente-4.c $(LDFLAGS) -lclaves

# ----------------------------------------------------
# FASE B: Versión Distribuida con Colas de Mensajes
# ----------------------------------------------------

# Servidor que usa la biblioteca local
servidor: servidor-mq.c libclaves.so comun.h
	$(CC) $(CFLAGS) -o servidor servidor-mq.c $(LDFLAGS) -lclaves -lrt

# Biblioteca proxy para el cliente distribuido
libproxyclaves.so: proxy-mq.c claves.h comun.h
	$(CC) $(CFLAGS) -fPIC -shared -o libproxyclaves.so proxy-mq.c -lrt

# Cliente distribuido básico (mismo app-cliente-1.c)
cliente_distribuido: app-cliente-1.c libproxyclaves.so
	$(CC) $(CFLAGS) -o cliente_distribuido app-cliente-1.c $(LDFLAGS) -lproxyclaves -lrt

# Clientes de prueba adicionales para versión distribuida
cliente_distribuido_test2: app-cliente-2.c libproxyclaves.so
	$(CC) $(CFLAGS) -o cliente_distribuido_test2 app-cliente-2.c $(LDFLAGS) -lproxyclaves -lrt

cliente_distribuido_test3: app-cliente-3.c libproxyclaves.so
	$(CC) $(CFLAGS) -o cliente_distribuido_test3 app-cliente-3.c $(LDFLAGS) -lproxyclaves -lrt

cliente_distribuido_test4: app-cliente-4.c libproxyclaves.so
	$(CC) $(CFLAGS) -o cliente_distribuido_test4 app-cliente-4.c $(LDFLAGS) -lproxyclaves -lrt

# ----------------------------------------------------
# LIMPIEZA
# ----------------------------------------------------
clean:
	rm -f *.o *.so cliente_local servidor cliente_distribuido
	rm -f cliente_local_test2 cliente_local_test4
	rm -f cliente_distribuido_test2 cliente_distribuido_test3 cliente_distribuido_test4

# ----------------------------------------------------
# LIMPIEZA COMPLETA (incluye colas de mensajes)
# ----------------------------------------------------
clean_all: clean
	@echo "Limpiando colas de mensajes..."
	@-rm -f /dev/mqueue/cola_servidor_claves 2>/dev/null || true
	@-rm -f /dev/mqueue/cola_cliente_* 2>/dev/null || true

# ----------------------------------------------------
# AYUDA: Muestra cómo usar el Makefile
# ----------------------------------------------------
help:
	@echo "=== SISTEMA DE TUPLAS CLAVE-VALOR ==="
	@echo ""
	@echo "COMPILACIÓN:"
	@echo "  make all              : Compila todo"
	@echo "  make clean             : Limpia ejecutables y librerías"
	@echo "  make clean_all         : Limpia también las colas de mensajes"
	@echo ""
	@echo "EJECUCIÓN - VERSIÓN LOCAL:"
	@echo "  ./cliente_local        : Prueba básica"
	@echo "  ./cliente_local_test2  : Prueba de casos de error"
	@echo "  ./cliente_local_test4  : Prueba de casos límite"
	@echo ""
	@echo "EJECUCIÓN - VERSIÓN DISTRIBUIDA:"
	@echo "  Terminal 1: ./servidor"
	@echo "  Terminal 2: ./cliente_distribuido       : Prueba básica"
	@echo "  Terminal 2: ./cliente_distribuido_test2 : Prueba de errores"
	@echo "  Terminal 2: ./cliente_distribuido_test3 : Prueba de concurrencia"
	@echo "  Terminal 2: ./cliente_distribuido_test4 : Prueba de casos límite"
	@echo ""
	@echo "NOTA: Para la versión distribuida, el servidor debe estar ejecutándose"
	@echo "      antes de lanzar cualquier cliente."