#include "mjep.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>

void listen_for_server_messages(void *server_socket) {
  char buffer[BUFFER_SIZE];
  char header[BUFFER_SIZE_HEADER];
  char body[MAX_LEN_USERNAME * BACKLOG];
  int *s_socket = (int *)server_socket;
  while (1) {
    bzero((void *)buffer, sizeof(buffer));
    if (recv(*s_socket, buffer, sizeof(buffer), 0) < 0) {
      printf("Unable to receive the message...\n");
    }
    uncapsulate_client(buffer, header, body);
    if ((strcmp(header, "ACK")) == 0) { // ask to wich user to connect
      printf("Users available to connect: \n");
      printf("%s\n", body);
      bzero((void *)buffer, sizeof(buffer));
      printf("-1 to refresh: \n");
      printf("Connect with: \n");
    } else if ((strcmp(header, "NACK")) == 0) {
      printf("NACK recieved\n");
      printf("Send a key to RESTART...\n");
      exit(0);
      break;
    } else if ((strcmp(header, "MESSAGE") == 0)) { // ask for message
      printf(">%s\n", body); // print message from the other user
    } else if ((strcmp(header, "DISCONNECT") == 0)) {
      printf("Disconnecting chat with client socket %d\n", *s_socket);
      break;
    }
  }
  shutdown(*s_socket, SHUT_RDWR);
  pthread_exit(NULL);
}

void listen_for_client_messages(void *server_socket) {
  char buffer[BUFFER_SIZE];
  char *user_input;
  int flag = 0;
  user_input = "-1";
  int *s_socket = (int *)server_socket;
  int n;
  while (1) {
    if (flag != 0) {
      bzero((void *)buffer, sizeof(buffer));
      break;
    }
    bzero((void *)buffer, sizeof(buffer));
    printf("Asking for index of client to chat...\n");
    n = 0;
    while ((buffer[n++] = getchar()) != '\n')
      ;
    buffer[strcspn(buffer, "\n")] = '\0';
    user_input = buffer;
    flag = strcmp(user_input, "-1");
    encapsulate_connect(buffer);
    if ((send(*s_socket, buffer, sizeof(buffer), 0)) < 0) {
      printf("Message not sent...\n");
    }
  }
  while (1) { // Ask for chat messages
    n = 0;
    /* printf("Asking for client messages...\n"); */
    while ((buffer[n++] = getchar()) != '\n')
      ;
    encapsulate_message(buffer);
    if ((send(*s_socket, buffer, sizeof(buffer), 0)) < 0) {
      printf("Message not sent...\n");
    }
  }
}

int main() {
  char hostname[30];
  // printf("Enter the ip address: \n");
  // scanf("%s", hostname);
  strcpy(hostname, "127.0.0.1");

  int sockfd, connfd;
  struct sockaddr_in servaddr, cli;

  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("socket creation failed...\n");
    exit(0);
  } else
    printf("Socket successfully created..\n");
  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(hostname);
  servaddr.sin_port = htons(PORT);

  // connect the client socket to server socket
  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
    printf("connection with the server failed...\n");
    exit(0);
  } else
    printf("connected to the server..\n");

  char buffer[MAX_LEN_USERNAME * BACKLOG];
  printf("Username: ");
  int n = 0;
  while ((buffer[n++] = getchar()) != '\n')
    ;
  encapsulate_register(buffer);

  if ((send(sockfd, buffer, sizeof(buffer), 0)) < 0) {
    printf("Message not sent...\n");
  }
  // registered

  pthread_t listen_server_thread;
  pthread_create(&listen_server_thread, NULL,
                 (void *)listen_for_server_messages, (void *)&sockfd);
  pthread_detach(listen_server_thread);
  listen_for_client_messages((void *)&sockfd);

  close(sockfd);
  printf("Socket closed...\n");
  exit(0);
}
