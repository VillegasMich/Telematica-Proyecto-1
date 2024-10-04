#include "mjep.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
  int client_1;
  int client_2;
} SocketPair;

void manage_chat(void *arg) {
  char buff[BUFFER_SIZE]; // maximo del mensaje
  char buff_2[sizeof(buff)];
  SocketPair *sockets =
      (SocketPair *)arg;            // convert the argument to a SocketPair
  int client_1 = sockets->client_1; // recive messages
  int client_2 = sockets->client_2; // send messages
  printf("Chatting \n");
  strcpy(buff, "connected");
  encapsulate_message(buff); // change for encapsulate ACK
  if ((send(client_1, buff, sizeof(buff), 0)) < 0) {
    printf("Message not sent...\n");
  }
  while (1) {
    bzero((void *)buff, BUFFER_SIZE);
    bzero((void *)buff_2, BUFFER_SIZE);
    int client_status = read_client(client_1, buff);
    if (client_status >= 0) {
      int status = uncapsulate_server(buff, NULL, -1, client_1, client_2);
      if (status == -1) {
        break;
      }
    } else {
      shutdown(client_1, SHUT_RDWR);
      pthread_exit(NULL);
    }

    bzero((void *)buff, sizeof(buff));
  }
}

void send_users(client *client_array, char *response, int index) {
  for (int i = 0; i < BACKLOG; i++) {
    if (client_array[i].username != NULL && i != index) {
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
    send_users(client_array, response, index);
    client_array[index].socket = client_socket;
  }
  encapsulate_ack(response);
  send(client_socket, response,
       (MAX_LEN_USERNAME * BACKLOG) + BUFFER_SIZE_HEADER, 0);
}

void manage_connect(char *body, client *client_array, int index,
                    int *client_socket) {
  if ((strcmp(body, "-1") == 0)) // refresh
  {

    char response[MAX_LEN_USERNAME * BACKLOG] = {0};
    send_users(client_array, response, index);
    encapsulate_ack(response);
    send(*client_socket, response,
         (MAX_LEN_USERNAME * BACKLOG) + BUFFER_SIZE_HEADER, 0);
  } else // connect with a user
  {
    client_array[index].chatting = atoi(body);
    if (index == atoi(body)) {
      // Si el numero que el ingresa es mayor que el BACKLOG entonces hay nack
      printf("Can't chat with yourself\n");
      client_array[index].chatting = -1;
      char response[BUFFER_SIZE_HEADER] = {0};
      encapsulate_nack(response);
      send(*client_socket, response, BUFFER_SIZE_HEADER, 0);
      return;
    }
    int i = 0;
    printf("Esperando a que el otro usuario se conecte\n");
    for (;;) {
      if (i == BACKLOG) {
        // numero de repeticiones de busqueda
        i = 0;
      }
      if (client_array[i].chatting == index && i != index) {
        printf("Found a chat to create %d and %d\n", index, i);
        SocketPair *sockets = (SocketPair *)malloc(sizeof(SocketPair));
        sockets->client_1 = client_array[index].socket;
        sockets->client_2 = client_array[i].socket;
        client_array[index].chatting = -1;
        client_array[i].chatting = -1;
        manage_chat((void *)sockets);
        break;
      }
      i++;
    }
  }
}

void connect_to_server(int *client_socket) {
  char hostname[15];
  strcpy(hostname, HOSTNAME);

  struct sockaddr_in servaddr, cli;

  // socket create and verification
  *client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (*client_socket == -1) {
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
  if (connect(*client_socket, (struct sockaddr *)&servaddr, sizeof(servaddr)) !=
      0) {
    printf("connection with the server failed...\n");
    exit(0);
  } else
    printf("connected to the server..\n");
}

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

int analyze_header_server(char *header, char *body, client *client_array,
                          int index, int client_socket, int client_socket_2) {
  if ((strcmp(header, "REGISTER") == 0)) {
    manage_register(body, client_array, index, client_socket);
    printf("-----registered----- \n");
  } else if ((strcmp(header, "CONNECT") == 0)) {
    manage_connect(body, client_array, index, &client_socket);
  } else if ((strcmp(header, "MESSAGE") == 0)) {
    char response[BUFFER_SIZE];
    strcpy(response, body);
    encapsulate_message(response);
    if ((send(client_socket_2, response, sizeof(response), 0)) < 0) {
      printf("Message not sent...\n");
    }
  } else if ((strcmp(header, "EXIT") == 0)) {
    char response[BUFFER_SIZE];
    strcpy(response, "The other client has exited the chat, please exit...");
    encapsulate_exit(response);
    if ((send(client_socket_2, response, sizeof(response), 0)) < 0) {
      printf("Message not sent...\n");
    }
    return -1;
  } else if ((strcmp(header, "DISCONNECT") == 0)) {
    // TODO: Remove from array
    printf("Disconnecting chat with client socket %d\n", client_socket);
    char response[BUFFER_SIZE];
    strcpy(response, "The other client has exited the chat, please exit...");
    encapsulate_exit(response);
    if ((send(client_socket_2, response, sizeof(response), 0)) < 0) {
      printf("Message not sent...\n");
    }
    return -1;
  }
  return 0;
}

int uncapsulate_server(char *buff, client *client_array, int index,
                       int client_socket, int client_socket_2) {
  char header[BUFFER_SIZE_HEADER], body[MAX_LEN_USERNAME * BACKLOG];
  char *token;
  token = strtok(buff, " ");
  strcpy(header, token);
  token = strtok(NULL, header);
  strcpy(body, token);
  body[strcspn(body, "\n")] = '\0';
  return analyze_header_server(header, body, client_array, index, client_socket,
                               client_socket_2);
}

int read_client(int client_socket, char *buff) {
  int recv_len = recv(client_socket, buff, BUFFER_SIZE, 0);
  if (recv_len == 0) {
    printf("Client disconnected %d\n", client_socket);
    // TODO: Remove from array
    return -1;
  }
  if (recv_len < 0) {
    perror("recv failed...\n");
    // TODO: Remove from array
    return -1;
  };
  return recv_len;
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

void encapsulate_nack(char *msg) {
  char encapsulated_msg[(MAX_LEN_USERNAME * BACKLOG) + BUFFER_SIZE_HEADER] =
      "NACK ";
  strcat(encapsulated_msg, msg);
  strcpy(msg, encapsulated_msg);
}

int analize_header_client(char *buffer, char *header, char *body) {
  if ((strcmp(header, "ACK")) == 0) { // ask to wich user to connect
    printf("\033[2J\033[1;1H");
    printf("Type 'disconnect' in a chat to end the program\n");
    printf("Type 'exit' in a chat to exit that chat\n");
    printf("Users available to connect: \n");
    printf("-1 to refresh: \n");
    printf("%s\n", body);
    bzero((void *)buffer, sizeof(buffer));
    printf("Connect with: \n");
  } else if ((strcmp(header, "NACK")) == 0) {
    printf("NACK recieved\n");
    printf("Send a key to RESTART...\n");
    // TODO: eliminate from clients array
    return -1;
  } else if ((strcmp(header, "MESSAGE") == 0)) { // ask for message
    if (strcmp(body, "connected") == 0)
      printf("\033[2J\033[1;1H"); // when connect clear the console
    printf(">>> %s\n", body);     // print message from the other user
  } else if ((strcmp(header, "DISCONNECT") == 0)) {
    printf("Disconnecting chat with client\n");
    printf("Send a key to RESTART...\n");
    // TODO: eliminate from clients array
    return -1;
  } else if ((strcmp(header, "EXIT") == 0)) {
    printf(">>> %s\n", body); // print message from the other user
  }
  return 0;
}

int uncapsulate_client(char *buff, char *header, char *body) {
  char *space = strchr(buff, ' ');
  if (space != NULL) {
    *space = '\0'; // terminate the header string
    strcpy(header, buff);
    strcpy(body, space + 1); // copy the body string
  } else {
    strcpy(header, buff);
    body[0] = '\0'; // empty body string
  }
  int result = analize_header_client(buff, header, body);
  return result;
}
