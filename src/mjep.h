#ifndef MJEP_H
#define MJEP_H
#define BUFFER_SIZE_MSG 500   // Buffer size of raw message
#define BUFFER_SIZE_HEADER 15 // Buffer size of header
#define PORT 8080             // Port of the protocol
#define BUFFER_SIZE BUFFER_SIZE_HEADER + BUFFER_SIZE_MSG
#define MAX_LEN_USERNAME 15 // Max length of username
#define TIMEUOT_CONN 10000  // 10 seconds time out for try connection

typedef struct {
  char *username;
  int socket;
  int chatting;
} client;

void connect_to_server(int *sockfd);
void initialize_conenction(int *sockfd);
void accept_connection(int *server_socket, int *client_socket);
void encapsulate_register(char *msg);
void encapsulate_connect(char *msg);
void encapsulate_message(char *msg);
void encapsulate_exit(char *msg);
void encapsulate_ack(char *msg);
void encapsulate_nack(char *msg);
void encapsulate_disconnect(char *msg);
int uncapsulate_server(char *buff, client *client_array, int index,
                       int client_socket, int client_socket_2);
int uncapsulate_client(char *buff, char *header, char *body);
int read_socket(int client_socket, char *buff, client *client_array, int index);
#endif
