
#include "funcionesSocket.h"

#define backlog 10 //cantidad de conexiones en la cola

int verificarErrorSocket(int socket) {
	if (socket == -1) {
		perror("Error de socket");
		exit(-1);
	} else {
		return 0;
	}
}
int verificarErrorSetsockopt(int socket) {
	int yes = 1;
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("Error de setsockopt");
		exit(-1);
	} else {
		return 0;
	}
}
int verificarErrorBind(int socket, struct sockaddr_in mySocket) {
	if (bind(socket, (struct sockaddr *) &mySocket, sizeof(mySocket)) == -1) {
		perror("Error de bind");
		exit(-1);
	} else {
		return 0;
	}
}
int verificarErrorListen(int socket) {
	if (listen(socket, backlog) == -1) {
		perror("Error de listen");
		exit(-1);
	} else {
		return 0;
	}
}
