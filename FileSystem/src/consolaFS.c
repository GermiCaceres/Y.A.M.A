#include "consolaFS.h"


#define maxSize 256

typedef struct {
  int flag; 
  char *nombre;     /* Nombre de la funcion */
} command;


command comandos[] = {
 { 1,"format"},
 { 2,"rm"},
 { 3,"rename"},
 { 4,"mv"},
 { 5,"cat"},
 { 6,"mkdir"},
 { 7,"cpfrom"},
 { 8,"cpto"},
 { 9,"cpblock"},
 { 10,"md5"},
 { 11,"ls"},
 { 12,"info"}
};


void imprimirComandos(){
  puts("\n");
  puts("*************************************************\n");
  puts("*                                               *\n");
  puts("*   Elegir para ejecutar dicho comando:         *\n");
  puts("*                                               *\n");
  puts("*************************************************\n");
  puts("\n");

  puts("format: Formatear el FileSystem. \n");
  puts("rm: Eliminar archivo/directorio/nodo/bloque. \n");
  puts("rename: Renombrar un archivo o directorio. \n");
  puts("mv: Mover un archivo o directorio. \n");
  puts("cat: Mostrar el contenido del archivo como texto plano. \n");
  puts("mkdir: Crear un directorio. \n");
  puts("cpfrom: Copiar un archivo local al yamafs. \n");
  puts("cpto: Copiar un archivo local al yamafs. \n");
  puts("cpblock: Crear copia de un bloque de un archivo. \n");
  puts("md5: Solicitar MD5 de un archivo del yamafs. \n");
  puts("ls Listar los archivos de un directorio. \n");
  puts("info: Mostrar información de un archivo. \n");
  
}



void analizarComando(char * linea){

  int i;
  int comandoNativo;
  int limite = (sizeof(comandos)/sizeof(comandos[0]));
  command comandoAux;

  char ** comandoDesarmado = string_split(linea," "); 
  char * primerPos = comandoDesarmado[0];

  for(i=0; i < limite; i++){
    comandoAux = comandos[i];

    if( strcmp(primerPos, comandoAux.nombre) == 0){
      comandoNativo = comandoAux.flag;
      }
  }


  switch(comandoNativo){

      case 1:{
        printf("Formateando FileSystem.\n");
        break;
      } 

      case 2:{
        //
        // Ver bien como hacer estos comandos despues
        //
        printf("Comando en arreglo! Todavia no se puede ejecutar! (2)\n");
      }
      break;


      case 3:{
        char * comandoNuevo = string_new();

        char * nombreArchivoViejo = comandoDesarmado[1];
        char * nombreArchivoNuevo = comandoDesarmado[2];

        string_append(&comandoNuevo,"mv ");
        string_append(&comandoNuevo,nombreArchivoViejo);
        string_append(&comandoNuevo," ");
        string_append(&comandoNuevo,nombreArchivoNuevo);


        system(comandoNuevo); 
        printf("\n");
    
      }
      break; 

      case 4:{
        system(linea); 
        printf("\n");

      }
      break;

      case 5:{  
        system(linea);
        printf("\n");
      }
      break;

      case 6:{
        system(linea);
        printf("\n");
      }
      break;

       case 7:{
        //
        // Ver bien como hacer estos comandos despues
        //
        printf("Comando en arreglo! Todavia no se puede ejecutar! (7)\n");
      }
      break;

      case 8:{
        //
        // Ver bien como hacer estos comandos despues
        //
        printf("Comando en arreglo! Todavia no se puede ejecutar! (8)\n");
      }
      break;

      case 9:{
        //
        // Ver bien como hacer estos comandos despues
        //
        printf("Comando en arreglo! Todavia no se puede ejecutar! (9)\n");
      }
      break;

      case 10:{
        char * comandoNuevo = string_new();

        char * nombreArchivoViejo = comandoDesarmado[1];

        string_append(&comandoNuevo,"md5sum ");
        string_append(&comandoNuevo,nombreArchivoViejo);

        system(comandoNuevo);
        printf("\n");
      }
      break;

      case 11:{
        system(linea);
        printf("\n");
      }
      break;

      case 12:{
        char * comandoNuevo = string_new();

        char * nombreArchivoViejo = comandoDesarmado[1];

        string_append(&comandoNuevo,"ls -l -h ");
        string_append(&comandoNuevo,nombreArchivoViejo);

        system(comandoNuevo);
        printf("\n");
      }
      break;

      default:
            printf("Comando no reconocido.\n");
            break;

    }

}



void consolaFS(){

  char * linea = string_new();

  if(linea == NULL){
    printf("Problemas creando cadena.");
  }
 
  imprimirComandos();

  printf("\n");
        
  while(1) {
    linea = readline(">>");   


    if(linea)
    	add_history(linea);

    if(!strncmp(linea, "exit", 4)) {
       free(linea);
       break;
    }

    analizarComando(linea); 

  }

}