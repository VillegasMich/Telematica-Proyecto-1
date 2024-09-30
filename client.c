#include "mjep.h"

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
  char buffer[115];
   printf("Username: ");
   int n = 0;
  while ((buffer[n++] = getchar()) != '\n')
                ;
  if ((send(sockfd, buffer, sizeof(buffer), 0)) < 0) {
      printf("Message not sent...\n");
    }

  // close the socket
  close(sockfd);
}