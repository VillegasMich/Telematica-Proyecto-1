#include "config.h"
#include "mjep.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>

typedef struct {
  int client_socket;
  client *clients_array;
  int index;
} thread_args;

void manage_client(void *arg) {
  thread_args *args = (thread_args *)arg;
  int own_client_socket =
      args->client_socket; // extract the values becouse other threads
  int index = args->index; // extract the values becouse other threads
  while (1) {
    int client_status = 0;
    char *buff = malloc(BUFFER_SIZE);
    bzero((void *)buff, BUFFER_SIZE);
    client_status =
        read_socket(own_client_socket, buff, args->clients_array, index);
    if (client_status >= 0) {
      uncapsulate_server(buff, args->clients_array, index, own_client_socket,
                         -1);
      free(buff);
    } else {
      free(buff);
      pthread_exit(NULL);
    }
  }
}

int main() {
  pthread_t thread_pool[BACKLOG];
  client clients_array[BACKLOG];
  int server_socket, n_thread, client_socket, index;
  thread_args t_arg;

  n_thread = 0;
  index = 0;

  for (int i = 0; i < BACKLOG; i++) {
    clients_array[i].username = NULL;
    clients_array[i].socket = 0;
    clients_array[i].chatting = -1;
  }

  t_arg.clients_array = clients_array;

  initialize_conenction(&server_socket);

  while (n_thread < BACKLOG) {
    accept_connection(&server_socket, &client_socket);
    t_arg.client_socket = client_socket;
    t_arg.index = index;
    /* printf("Thread -> %lu\n", thread_pool[n_thread]); */
    if ((pthread_create(&thread_pool[n_thread], NULL, (void *)manage_client,
                        &t_arg)) < 0) {
      printf("Error creating the thread");
      /* exit(0); */
    }
    pthread_detach(thread_pool[n_thread]);
    n_thread++;
    index++;
  }

  return 0;
}
