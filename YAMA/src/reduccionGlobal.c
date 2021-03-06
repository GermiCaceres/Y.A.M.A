#include "reduccionGlobal.h"
#include <commons/collections/list.h>

bool sePuedeHacerReduccionGlobal(int nroMaster){
	bool esDeMaster(administracionYAMA* admin){
		return admin->nroMaster == nroMaster;
	}
	bool estaTerminado(administracionYAMA* admin){
		return admin->estado == FINALIZADO || admin->estado == FALLO;
	}
	pthread_mutex_lock(&semTablaEstados);
	t_list* listaDeMaster = list_filter(tablaDeEstados, (void*)esDeMaster);
	bool sePuede = list_all_satisfy(listaDeMaster, (void*)estaTerminado);
	pthread_mutex_unlock(&semTablaEstados);
	list_destroy(listaDeMaster);
	return sePuede;
}


t_list *filtrarReduccionesDelNodo(uint32_t nroMaster){
	bool esRLocalTerminada(administracionYAMA* admin){
		return admin->etapa == REDUCCION_LOCAL && admin->nroMaster == nroMaster;
	}
	pthread_mutex_lock(&semTablaEstados);
	t_list* listaDeMaster = list_filter(tablaDeEstados, (void*)esRLocalTerminada);
	pthread_mutex_unlock(&semTablaEstados);
	return listaDeMaster;
}

t_list* obtenerConexionesDeNodos(t_list* listaDeMaster, char* nodoEncargado){
	uint32_t posicion;
	t_list* listaDeConexiones = list_create();
	for(posicion = 0; posicion < list_size(listaDeMaster); posicion++){
		administracionYAMA* admin = list_get(listaDeMaster, posicion);
		conexionNodo* conect = generarConexionNodo();
		conect->nombreNodo = string_new();
		string_append(&conect->nombreNodo, admin->nombreNodo);
		obtenerIPYPuerto(conect);
		if(conect->ipNodo == NULL && conect->puertoNodo == -1){
			return NULL;
		}
		list_add(listaDeConexiones, conect);
	}
	return listaDeConexiones;
}

int cargarReduccionGlobal(int socketMaster, int nroMaster, t_list* listaDeMaster){
	administracionYAMA* nuevaReduccionG = generarAdministracion(obtenerJobDeNodo(listaDeMaster),nroMaster, REDUCCION_GLOBAL, obtenerNombreTemporalGlobal());
	nuevaReduccionG->nroBloque = 0;
	nuevaReduccionG->nroBloqueFile = -1;
	t_list* listaDeConexiones = obtenerConexionesDeNodos(listaDeMaster, nuevaReduccionG->nombreNodo);
	uint32_t tamanioMensaje = obtenerTamanioReduGlobal(nuevaReduccionG, listaDeConexiones, listaDeMaster);
	log_info(loggerYAMA, "REDUCCION GLOBAL - BALANCEO DE CARGAS - ALGORITMO %s", ALGORITMO_BALANCEO);
	nuevaReduccionG->nombreNodo = balancearReduccionGlobal(listaDeMaster);
	log_info(loggerYAMA, "REDUCCION GLOBAL - MASTER %d - NODO %s", nroMaster, nuevaReduccionG->nombreNodo);
	if(listaDeConexiones == NULL){
		log_error(loggerYAMA, "ERROR - IP Y PUERTO DE NODOS");
		return -1;
	}
	actualizarWLRGlobal(nuevaReduccionG->nombreNodo, list_size(listaDeMaster));
	log_trace(loggerYAMA, "ACTUALIZACION DE WL");
	void* infoGlobalSerializada = serializarInfoReduccionGlobal(nuevaReduccionG, listaDeConexiones, listaDeMaster);
	log_info(loggerYAMA, "REDUCCION GLOBAL - DATOS SERIALIZADOS - MASTER %d", nroMaster);
	sendRemasterizado(socketMaster, REDUCCION_GLOBAL, tamanioMensaje, infoGlobalSerializada);
	log_debug(loggerYAMA, "REDUCCION GLOBAL - INFORMACION ENVIADOS - MASTER %d", nroMaster);
	pthread_mutex_lock(&semTablaEstados);
	list_add(tablaDeEstados, nuevaReduccionG);
	pthread_mutex_unlock(&semTablaEstados);
	free(infoGlobalSerializada);
	list_destroy_and_destroy_elements(listaDeConexiones, (void*)liberarConexion);
	return 1;
}

void terminarReduccionGlobal(uint32_t nroMaster){
	bool esReduGlobalMaster(administracionYAMA* admin){
		return admin->nroMaster == nroMaster && admin->etapa == REDUCCION_GLOBAL && admin->estado == EN_PROCESO;
	}
	pthread_mutex_lock(&semTablaEstados);
	administracionYAMA* admin = list_find(tablaDeEstados, (void*)esReduGlobalMaster);
	admin->estado = FINALIZADO;
	pthread_mutex_unlock(&semTablaEstados);
}

char* obtenerNombreArchivoReduGlobal(int nroMaster){
	bool esReduGlobal(administracionYAMA* admin){
		return admin->nroMaster == nroMaster && admin->etapa == REDUCCION_GLOBAL;
	}
	char* nombreArch = string_new();
	pthread_mutex_lock(&semTablaEstados);
	administracionYAMA* admin = list_find(tablaDeEstados, (void*)esReduGlobal);
	string_append(&nombreArch, admin->nameFile);
	pthread_mutex_unlock(&semTablaEstados);
	return nombreArch;
}

int almacenadoFinal(int socketMaster, uint32_t nroMaster){
	char* nodoEncargado = buscarNodoEncargado(nroMaster);
	log_info(loggerYAMA, "ENCARGADO DE ALMACENAMIENTO - %s.", nodoEncargado);
	conexionNodo* conect = generarConexionNodo();
	conect->nombreNodo = string_new();
	string_append(&conect->nombreNodo, nodoEncargado);
	free(nodoEncargado);
	obtenerIPYPuerto(conect);
	if(conect->ipNodo == NULL || conect->puertoNodo == 0){
		return -1;
	}
	char* nombreArchReduGlobal = obtenerNombreArchivoReduGlobal(nroMaster);
	log_info(loggerYAMA, "ALMACENAMIENTO FINAL - SERIALIZACION DE DATOS - MASTER %d", nroMaster);
	void* infoAlmacenadoFinal = serializarInfoAlmacenamientoFinal(conect, nombreArchReduGlobal);
	sendRemasterizado(socketMaster, ALMACENAMIENTO_FINAL, obtenerTamanioInfoAlmacenamientoFinal(conect, nombreArchReduGlobal), infoAlmacenadoFinal);
	log_info(loggerYAMA, "ALMACENAMIENTO FINAL - INFORMACION ENVIADA - MASTER %d", nroMaster);
	free(infoAlmacenadoFinal);
	liberarConexion(conect);
	free(nombreArchReduGlobal);
	return 1;
}

void reestablecerWLGlobal(int nroMaster, int flag){
	bool esTransformacion(administracionYAMA* admin){
		return admin->etapa == TRANSFORMACION && admin->estado == FINALIZADO && admin->nroMaster == nroMaster;
	}
	bool esReduLocal(administracionYAMA* admin){
		return admin->etapa == REDUCCION_LOCAL && admin->estado == FINALIZADO && admin->nroMaster == nroMaster;
	}
	t_list* listaTransformaciones;
	t_list* listaReduccionesLocales;
	pthread_mutex_lock(&semNodosSistema);
	listaTransformaciones = list_filter(tablaDeEstados, (void*)esTransformacion);
	listaReduccionesLocales = list_filter(tablaDeEstados, (void*)esReduLocal);
	pthread_mutex_unlock(&semNodosSistema);
	eliminarTrabajosLocales(listaTransformaciones);
	if(flag == FINALIZO){
		eliminarTrabajosGlobales(nroMaster, listaReduccionesLocales);
	}
	list_destroy(listaTransformaciones);
	list_destroy(listaReduccionesLocales);
}

void fallaReduccionGlobal(int nroMaster){
	bool esReduccionGlobal(administracionYAMA* admin){
		return admin->nroMaster == nroMaster && admin->etapa == REDUCCION_GLOBAL;
	}
	pthread_mutex_lock(&semTablaEstados);
	administracionYAMA* admin = list_find(tablaDeEstados, (void*)esReduccionGlobal);
	admin->estado = FALLO;
	pthread_mutex_unlock(&semTablaEstados);
}
