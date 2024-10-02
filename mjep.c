#include "mjep.h"
#include <stdio.h>
#include <string.h>

void send_users(client *client_array, char *response) {
  for (int i = 0; i < BACKLOG; i++) {
    if (client_array[i].username != NULL) {
      char i_str[10];
      sprintf(i_str, "%d-", i);
      strcat(response, i_str);
      strcat(response, client_array[i].username);
      strcat(response, " \n");
    }
  }
}

void manage_register(char *body, client *client_array, int index,
                     int client_socket) {
  char response[MAX_LEN_USERNAME * BACKLOG] = {0};
  client_array[index].username = malloc(strlen(body) + 1);
  if (client_array[index].username == NULL || index >= BACKLOG) {
    strcpy(response, "0");
  } else {
    strcpy(client_array[index].username, body);
    send_users(client_array, response);
    client_array[index].socket = client_socket;
  }
  encapsulate_ack(response);
  send(client_socket, response,
       (MAX_LEN_USERNAME * BACKLOG) + BUFFER_SIZE_HEADER, 0);
  for (int i = 0; i < BACKLOG; i++) {
    if (client_array[i].username != NULL) {
      printf("%s \n", client_array[i].username);
    }
  }
}

void connect_to_server() { printf("Connecting to server\n"); }

// initialize the server Socket - main
void initialize_conenction(int *server_socket) {
  printf("Creating connection\n");
  struct sockaddr_in servaddr;
  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(PORT);

  // socket create and verification
  *server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (*server_socket == -1) {
    printf("socket 1 creation failed...\n");
    exit(0);
  } else
    printf("Socket 1 successfully created..\n");

  // Binding newly created socket to given IP and verification
  if ((bind(*server_socket, (struct sockaddr *)&servaddr, sizeof(servaddr))) !=
      0) {
    printf("socket 1 bind failed...\n");
    exit(0);
  } else
    printf("Socket 1 successfully binded..\n");

  // Now server is ready to listen and verification
  if ((listen(*server_socket, 100)) != 0) {
    printf("Listen 1 failed...\n");
    exit(0);
  } else {
    printf("Server 1 listening..\n");
  }
}

void accept_connection(int *server_socket, int *client_socket) {
  struct sockaddr client;
  unsigned int len;
  len = sizeof(struct sockaddr_in);
  *client_socket = accept(*server_socket, &client, &len);
  if (*client_socket < 0) {
    printf("server accept failed for client ...\n");
    exit(0);
  } else {
    printf("server accepted client ... \n");
    // write(*client_socket, "Connected", sizeof("Connected"));
  }
}

void analyze_header(char *header, char *body, client *client_array, int index,
                    int client_socket) {
  /* printf("Analizing with n_thread of %d \n", n_thread); */
  if ((strcmp(header, "REGISTER") == 0)) {
    manage_register(body, client_array, index, client_socket);
    printf("-----registered----- \n");

  } else if ((strcmp(header, "CONNECT") == 0)) {
    printf("CON\n");
    // Busca en la estructura de datos el body
    // Si lo encuentra crea la pareja de clientes
  } else if ((strcmp(header, "MESSAGE") == 0)) {
    printf("MESS\n");
    // Lee el body, lo encapsula y lo manda al otro cliente
  } else if ((strcmp(header, "EXIT") == 0)) {
    printf("EX\n");
    // Rompe la pareja de clientes
  } else if ((strcmp(header, "DISCONNECT") == 0)) {
    printf("DIS\n");
    // Mata al cliente
  }
}

void uncapsulate(char *buff, client *client_array, int index,
                 int client_socket) {
  char header[BUFFER_SIZE_HEADER], body[MAX_LEN_USERNAME * BACKLOG];
  char *token;
  token = strtok(buff, " ");
  strcpy(header, token);
  token = strtok(NULL, header);
  strcpy(body, token);
  body[strcspn(body, "\n")] = '\0';
  printf("header: %s body: %s \n", header, body);
  analyze_header(header, body, client_array, index, client_socket);
}

void read_client(int client_socket, char *buff) {
  bzero((void *)buff, BUFFER_SIZE);
  printf("Reading from socket %d\n", client_socket);
  recv(client_socket, buff, BUFFER_SIZE, 0);
}

void encapsulate_register(char *msg) {
  char encapsulated_msg[BUFFER_SIZE] = "REGISTER ";
  strcat(encapsulated_msg, msg);
  strcpy(msg, encapsulated_msg);
}

void encapsulate_connect(char *msg) {
  char encapsulated_msg[BUFFER_SIZE] = "CONNECT ";
  strcat(encapsulated_msg, msg);
  strcpy(msg, encapsulated_msg);
}

void encapsulate_message(char *msg) {
  char encapsulated_msg[BUFFER_SIZE] = "MESSAGE ";
  strcat(encapsulated_msg, msg);
  strcpy(msg, encapsulated_msg);
}

void encapsulate_exit(char *msg) {
  char encapsulated_msg[BUFFER_SIZE] = "EXIT ";
  strcat(encapsulated_msg, msg);
  strcpy(msg, encapsulated_msg);
}

void encapsulate_disconnect(char *msg) {
  char encapsulated_msg[BUFFER_SIZE] = "DISCONNECT ";
  strcat(encapsulated_msg, msg);
  strcpy(msg, encapsulated_msg);
}

void encapsulate_ack(char *msg) {
  char encapsulated_msg[(MAX_LEN_USERNAME * BACKLOG) + BUFFER_SIZE_HEADER] =
      "ACK ";
  strcat(encapsulated_msg, msg);
  strcpy(msg, encapsulated_msg);
}
