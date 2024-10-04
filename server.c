#include "mjep.h"
#include <stdio.h>
#include <threads.h>

typedef struct {
  int client_socket;
  client *clients_array;
  int index;
} thread_args;

/* El client_socket se modifica cuando entre otro thread */

void manage_client(void *arg) {
  /* char buff[BUFFER_SIZE]; */
  int iteration = 0;
  thread_args *args = (thread_args *)arg;
  int own_client_socket =
      args->client_socket; // extract the values becouse other threads
  int index = args->index; // extract the values becouse other threads
  while (1) {
    int client_status = 0;
    char *buff = malloc(BUFFER_SIZE);
    bzero((void *)buff, BUFFER_SIZE);
    client_status = read_client(own_client_socket, buff);
    if (client_status >= 0) {
      uncapsulate(buff, args->clients_array, index, own_client_socket);
      iteration++;
      free(buff);
    } else {
      shutdown(own_client_socket, SHUT_RDWR);
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

  while (1) {
    accept_connection(&server_socket, &client_socket);
    t_arg.client_socket = client_socket;
    t_arg.index = index;
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
