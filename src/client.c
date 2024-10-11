#include "colors.h"
#include "config.h"
#include "mjep.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void listen_for_server_messages(void *server_socket) {
  char buffer[BUFFER_SIZE];
  char header[BUFFER_SIZE_HEADER];
  char body[MAX_LEN_USERNAME * BACKLOG];
  int *s_socket = (int *)server_socket;
  while (1) {
    bzero((void *)buffer, BUFFER_SIZE);
    int socket_status = read_socket(*s_socket, buffer, NULL, -1);
    if (socket_status < 0) {
      break;
    }
    int result = uncapsulate_client(buffer, header, body);
    if (result == -1) {
      break;
    }
  }
  shutdown(*s_socket, SHUT_RDWR);
  pthread_exit(NULL);
}

void listen_for_client_messages(void *server_socket) {
  int *s_socket = (int *)server_socket;
  while (1) {
    char buffer[BUFFER_SIZE];
    char *user_input;
    user_input = "-1";
    int flag = 0;
    int n;
    while (1) {
      if (flag != 0) {
        bzero((void *)buffer, sizeof(buffer));
        break;
      }
      bzero((void *)buffer, sizeof(buffer));
      printf("Asking for index of client to chat...\n");
      printf("-1 to refresh: \n");
      n = 0;
      while ((buffer[n++] = getchar()) != '\n')
        ;
      buffer[strcspn(buffer, "\n")] = '\0';
      if (strcmp(buffer, "disconnect") == 0) {
        printf(YELLOW "Disconnecting from server...\n" RESET);
        encapsulate_disconnect(buffer);
        if ((send(*s_socket, buffer, sizeof(buffer), 0)) < 0) {
          printf(RED "Message not sent...\n" RESET);
        }
        shutdown(*s_socket, SHUT_RDWR);
        pthread_exit(NULL);
      }
      user_input = buffer;
      int num = atoi(user_input);
      if (num == 0 && user_input[0] != '0') {
        user_input = "-1";
        strcpy(buffer, "-1");
      }
      flag = strcmp(user_input, "-1");
      encapsulate_connect(buffer);
      if ((send(*s_socket, buffer, sizeof(buffer), 0)) < 0) {
        printf(RED "Message not sent...\n" RESET);
      }
    }
    while (1) { // Ask for chat messages
      n = 0;
      while ((buffer[n++] = getchar()) != '\n')
        ;
      buffer[strcspn(buffer, "\n")] = '\0';
      if (strlen(buffer) == 0) {
        printf(YELLOW "Enter a valid string\n" RESET);
        continue;
      }
      if (strcmp(buffer, "disconnect") == 0) {
        printf(YELLOW "Disconnecting from server...\n" RESET);
        encapsulate_disconnect(buffer);
        if ((send(*s_socket, buffer, sizeof(buffer), 0)) < 0) {
          printf(RED "Message not sent...\n" RESET);
        }
        shutdown(*s_socket, SHUT_RDWR);
        pthread_exit(NULL);
      }
      if (strcmp(buffer, "exit") == 0) {
        encapsulate_exit(buffer);
        if ((send(*s_socket, buffer, sizeof(buffer), 0)) < 0) {
          printf(RED "Message not sent...\n" RESET);
        }
        break;
      }
      encapsulate_message(buffer);
      if ((send(*s_socket, buffer, sizeof(buffer), 0)) < 0) {
        printf(RED "Message not sent...\n" RESET);
      }
    }
  }
}

int main() {
  int client_socket;
  connect_to_server(&client_socket);

  char buffer[MAX_LEN_USERNAME * BACKLOG];
  printf("Username: ");
  int n = 0;
  while ((buffer[n++] = getchar()) != '\n')
    ;
  encapsulate_register(buffer);

  if ((send(client_socket, buffer, sizeof(buffer), 0)) < 0) {
    printf(RED "Message not sent...\n" RESET);
  }
  // registered

  pthread_t listen_server_thread;
  pthread_create(&listen_server_thread, NULL,
                 (void *)listen_for_server_messages, (void *)&client_socket);
  pthread_detach(listen_server_thread);
  listen_for_client_messages((void *)&client_socket);

  close(client_socket);
  printf("Socket closed...\n");
  exit(0);
}
