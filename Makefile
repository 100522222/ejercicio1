CC = gcc
CFLAGS = -Wall -g -pthread
LDFLAGS = -L. -Wl,-rpath=./ 

# ¡Esta es la línea clave que le dice todo lo que tiene que construir!
all: libclaves.so cliente_local servidor libproxyclaves.so cliente_distribuido

# ----------------------------------------------------
# FASE A: Versión Local Monolítica
# ----------------------------------------------------
libclaves.so: claves.c claves.h
	$(CC) $(CFLAGS) -fPIC -shared -o libclaves.so claves.c

cliente_local: app_cliente.c libclaves.so
	$(CC) $(CFLAGS) -o cliente_local app_cliente.c $(LDFLAGS) -lclaves

# ----------------------------------------------------
# FASE B: Versión Distribuida
# ----------------------------------------------------
servidor: servidor-mq.c libclaves.so
	$(CC) $(CFLAGS) -o servidor servidor-mq.c $(LDFLAGS) -lclaves -lrt

libproxyclaves.so: proxy-mq.c claves.h comun.h
	$(CC) $(CFLAGS) -fPIC -shared -o libproxyclaves.so proxy-mq.c -lrt

cliente_distribuido: app_cliente.c libproxyclaves.so
	$(CC) $(CFLAGS) -o cliente_distribuido app_cliente.c $(LDFLAGS) -lproxyclaves -lrt

# ----------------------------------------------------
# LIMPIEZA
# ----------------------------------------------------
clean:
	rm -f *.o *.so cliente_local servidor cliente_distribuido