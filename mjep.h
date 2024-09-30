#ifndef MJEP_H
#define MJEP_H
#define BUFFER_SIZE_MSG 100
#define BUFFER_SIZE_HEADER 15
#define PORT 8080
#define BUFFER_SIZE BUFFER_SIZE_HEADER + BUFFER_SIZE_MSG
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()
#include<arpa/inet.h> // inet_addr()

void connect_to_server(); // client
void initialize_conenction(int *sockfd); // server
void accept_connection(int *server_socket, int *client_socket); // server
void encapsulate_register(char msg[BUFFER_SIZE_MSG]);
void encapsulate_connect(char msg[BUFFER_SIZE_MSG]);
void encapsulate_message(char msg[BUFFER_SIZE_MSG]);
void encapsulate_exit(char msg[BUFFER_SIZE_MSG]);
void encapsulate_disconnect(char msg[BUFFER_SIZE_MSG]);

void uncapsulate_msg(char *buff);
void read_client(int *client_socket, char *buff);
#endif
