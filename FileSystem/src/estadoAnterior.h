/*
 * estadoAnterior.h
 *
 *  Created on: 10/12/2017
 *      Author: utnso
 */

#include "estructuras.h"
#include "funcionesPath.h"
#include "funcionesConsola.h"
#include "principalesFS.h"

#ifndef ESTADOANTERIOR_H_
#define ESTADOANTERIOR_H_


void cargarEstructuraDirectorio(t_config* );
void cargarEstructuraBitmap(strNodo* );
void cargarEstructuraNodos(t_config* );
void cargarTablaArchivo(char* );
void cargarEstructuraArchivos(t_config* );
bool presentaUnEstadoAnterior();


#endif /* ESTADOANTERIOR_H_ */
