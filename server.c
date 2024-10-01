#include "mjep.h"

typedef struct {
    int* client_socket;
    client *clients_array;
    int n_thread;
} thread_args;

void manage_client(void *arg){
        char buff[BUFFER_SIZE];
        thread_args* args = (thread_args*)arg;
        read_client(args->client_socket, buff);
        uncapsulate(buff, args->clients_array, args->n_thread, args->client_socket);
        
        // for(int i=0; i < BACKLOG; i++){
        //     printf("username from thread: %s\n Socket-thread: %d\n", args->clients_array[i].username, args->clients_array[i].socket);
        // }
        
}

int main(){
    pthread_t thread_pool[BACKLOG];
    client clients_array[BACKLOG];
    int server_socket, n_thread;
    thread_args t_arg;

    for(int i=0; i < BACKLOG; i++){
        clients_array[i].username = NULL;
        clients_array[i].socket = 0;
        clients_array[i].chatting = 0;
    }


    t_arg.clients_array = clients_array;

    //initialize the server Socket - main socket
    initialize_conenction(&server_socket);

    while (1){
        int client_socket;
        accept_connection(&server_socket, &client_socket); 
        t_arg.client_socket = &client_socket;
        t_arg.n_thread = n_thread;
        if ((pthread_create(&thread_pool[n_thread], NULL, (void *)manage_client, &t_arg)) < 0){
            printf("Error creating the thread");
            exit(0);
        }
        pthread_detach(thread_pool[n_thread]);
        n_thread++;

    }
    
    return 0;
}
