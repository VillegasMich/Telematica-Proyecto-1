#ifndef MJEP_H
#define MJEP_H
#define BUFFER_SIZE_MSG 500
#define BUFFER_SIZE_HEADER 15
#define PORT 8080
#define BUFFER_SIZE BUFFER_SIZE_HEADER + BUFFER_SIZE_MSG
#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()
#define MAX_LEN_USERNAME 15
#define BACKLOG 10

typedef struct {
  char *username;
  int socket;
  int chatting;
} client;

void connect_to_server();                                       // client
void initialize_conenction(int *sockfd);                        // server
void accept_connection(int *server_socket, int *client_socket); // server
void encapsulate_register(char *msg);
void encapsulate_connect(char *msg);
void encapsulate_message(char *msg);
void encapsulate_exit(char *msg);
void encapsulate_ack(char *msg);
void encapsulate_nack(char *msg);
void encapsulate_disconnect(char *msg);
void uncapsulate(char *buff, client *client_array, int index,
                 int client_socket);
void uncapsulate_client(char *buff, char *header, char *body);
int read_client(int client_socket, char *buff);
#endif
