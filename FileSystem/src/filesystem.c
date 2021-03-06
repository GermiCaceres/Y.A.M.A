
#include "../../Biblioteca/src/configParser.c"
#include "../../Biblioteca/src/Socket.c"
#include "estructuras.h"
#include "principalesFS.h"
#include "consola.h"
#include <signal.h>
#include "estadoAnterior.h"

void iniciarModoDebug(char * pathConfig){
	t_config* configuracionFS = generarTConfig(pathConfig, 2);
	cargarConfigFS(configuracionFS);
}

void iniciarModoNormal(int argc, char **argv){
	t_config* configuracionFS = generarTConfig(argv[1], 2);
	cargarConfigFS(configuracionFS);
	chequearParametrosFS(argc, argv[2]);
}

int main(int argc, char **argv) {

	signal(SIGINT, handlerSIGINT);

//	iniciarModoDebug("src/off_filesystem.ini");
	iniciarModoNormal(argc,argv);

	loggerFileSystem = log_create("FileSystem.log", "FileSystem", 0, 0);

	// Genero el socket listener
	socketListener = iniciarServidor(PUERTO_ESCUCHA);
	socketMaximo = socketListener;

	FD_ZERO(&socketClientes);
	FD_ZERO(&socketClientesAuxiliares);
	FD_SET(socketListener, &socketClientes);

	// Inicio las estructuras tipo listas
	iniciarEstructuras();

	//Verifico si hay estado anterior
	if(presentaUnEstadoAnterior() == false){
		//Inicio directorios principales
		inicializarDirectoriosPrincipales();

		//Agrego la raiz directorio
		iniciarTablaDeDirectorios();


		estadoAnterior = false;
	}

	log_info(loggerFileSystem,"Estructuras iniciadas");

	// Creo el hilo consola
	pthread_create(&hiloConsolaFS, NULL, (void*) consolaFS, NULL);

	log_info(loggerFileSystem,"Consola iniciada");


	int socketCliente;
	int corte = 1;

	log_info(loggerFileSystem,"Esperando conexiones");

	// Funcion principal
	while(corte) {

		pthread_mutex_lock(&mutex);
		socketClientesAuxiliares = socketClientes;
		pthread_mutex_unlock(&mutex);

		if (select(socketMaximo + 1, &socketClientesAuxiliares, NULL, NULL,NULL) == -1) {
			log_error(loggerFileSystem, "No se pudo llevar a cabo el select");
			liberarMemoria();
			exit(-1);
		}

		for (socketCliente = 0; socketCliente <= socketMaximo; socketCliente++) {

			pthread_mutex_lock(&mutex);
			bool fd_isset = FD_ISSET(socketCliente, &socketClientesAuxiliares);

			if (fd_isset) {
				if (socketCliente == socketListener) {

					// Nueva conexion
					atenderConexion(socketCliente);

				} else {

					// Nueva notificacion
					atenderNotificacion(socketCliente);

				}

			}
			pthread_mutex_unlock(&mutex);

		}

	}

	liberarMemoria();

	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;

}
