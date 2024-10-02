#include "mjep.h"
#include <string.h>

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

  // function for chat
  char buffer[2000];
  printf("Username: ");
  int n = 0;
  while ((buffer[n++] = getchar()) != '\n')
    ;
  encapsulate_register(buffer);

  if ((send(sockfd, buffer, sizeof(buffer), 0)) < 0) {
    printf("Message not sent...\n");
  }

  // receive the message from server
  if (recv(sockfd, buffer, sizeof(buffer), 0) < 0) {
    printf("Unable to receive the message...\n");
  }
  // print the received message TODOS LOS DESENCAPSULAR
  char header[BUFFER_SIZE_HEADER], body[MAX_LEN_USERNAME * BACKLOG];
  char *token;
  token = strtok(buffer, " ");
  strcpy(header, token);
  token = strtok(NULL, header);
  strcpy(body, token);
  if ((strcmp(header, "ACK") == 0) && (strcmp(body, "0") != 0)) {
    printf("%s\n", body);
  } else {
  }

  // si es 0 le mandamos otra vez el register al server

  // close the socket
  close(sockfd);
}
