#include "mjep.h"

int main(){

    // creamos un hilo por cliente (JUAN CARLOS)

    int server_socket;

    //initialize the server Socket - main socket
    initialize_conenction(&server_socket);

    while (1){
        int client_socket;
        char buff[BUFFER_SIZE];

        printf("waiting for client: \n");
        accept_connection(&server_socket, &client_socket);
        read_client(&client_socket, buff);
        uncapsulate_msg(buff);
    }
    
    return 0;
}