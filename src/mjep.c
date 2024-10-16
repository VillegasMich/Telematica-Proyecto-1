#include "mjep.h"
#include "colors.h"
#include "config.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <locale.h>
#include <stdio.h>

typedef struct {
  int client_1;
  int client_2;
} SocketPair;

/*
 * This function manages the chat between two clients
 *
 * Input:
 *    - void *arg: The socket pair. Mandatory
 * */
void manage_chat(void *arg, client *client_array, int index) {
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
    int client_status = read_socket(client_1, buff, client_array, index);
    if (client_status >= 0) {
      int status =
          uncapsulate_server(buff, client_array, index, client_1, client_2);
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

/*
 * This function manages the disconnection of a client
 *
 * Input:
 *    - char *body: The body of the message
 *    - client *client_array: The array of clients
 *    - int index: The index of the client
 *    - int client_socket: The socket of the client
 * */
void manage_disconnect(client *client_array, int index, int client_socket) {
  if (client_array[index].username != NULL) {
    free(client_array[index].username);
    client_array[index].username = NULL;
    client_array[index].socket = 0;
    client_array[index].chatting = -1;
  }

  /* for (int j = index; j < BACKLOG - 1; ++j) { */
  /*   client_array[j].username = client_array[j + 1].username; */
  /*   client_array[j].socket = client_array[j + 1].socket; */
  /*   client_array[j].chatting = client_array[j + 1].chatting; */
  /* } */

  /* for (int i = 0; i < BACKLOG; ++i) { */
  /*   if (client_array[i].username != NULL) { */
  /*     printf("Client %d: %s\n", i + 1, client_array[i].username); */
  /*   } */
  /* } */
  close(client_socket);
}

/*
 * This function sends the list of users to the client (except himself)
 *
 * Input:
 *    - client *client_array: The array of clients
 *    - char *response: The response to send
 *    - int index: The index of the own client
 * */
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

/*
 * This function manages the register of a new user
 *
 * Input:
 *    - char *body: The body of the request
 *    - client *client_array: The array of clients
 *    - int index: The index of the own client
 *    - int client_socket: The socket of the client
 * */
void manage_register(char *body, client *client_array, int index,
                     int client_socket) {
  char response[MAX_LEN_USERNAME * BACKLOG] = {0};
  client_array[index].username = malloc(strlen(body) + 100);
  if (client_array[index].username == NULL || index >= BACKLOG) {
    strcpy(response, "0");
  } else {
    strcpy(client_array[index].username, body);
    send_users(client_array, response, index);
    client_array[index].socket = client_socket;
  }
  encapsulate_register(response);
  send(client_socket, response,
       (MAX_LEN_USERNAME * BACKLOG) + BUFFER_SIZE_HEADER, 0);
}

unsigned long millis() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

/*
 * This function manages the connect request
 *
 * Input:
 *    - char *body: The body of the request
 *    - client *client_array: The array of clients
 *    - int index: The index of the own client
 *    - int *client_socket: The socket of the client
 * */
void manage_connect(char *body, client *client_array, int index,
                    int *client_socket) {
  if ((strcmp(body, "-1") == 0)) // refresh
  {

    char response[MAX_LEN_USERNAME * BACKLOG] = {0};
    send_users(client_array, response, index);
    encapsulate_register(response);
    send(*client_socket, response,
         (MAX_LEN_USERNAME * BACKLOG) + BUFFER_SIZE_HEADER, 0);
  } else // connect with a user
  {
    int input_from_client = atoi(body);
    client_array[index].chatting = input_from_client;
    if (index == input_from_client) {
      client_array[index].chatting = -1;
      char response[BUFFER_SIZE] = {0};
      strcpy(response, "Can't chat with yourself");
      encapsulate_nack(response);
      int val = send(*client_socket, response, BUFFER_SIZE, 0);
      manage_disconnect(client_array, index, *client_socket);
      if (val < 0) {
        printf("Message not sent...\n");
      }
      return;
    }
    if (input_from_client < 0 || input_from_client >= BACKLOG) {
      client_array[index].chatting = -1;
      char response[BUFFER_SIZE] = {0};
      strcpy(response, "Incorrect index, please try, again");
      encapsulate_nack(response);
      int val = send(*client_socket, response, BUFFER_SIZE, 0);
      manage_disconnect(client_array, index, *client_socket);
      if (val < 0) {
        printf("Message not sent...\n");
      }
      return;
    }
    if (client_array[input_from_client].username == NULL) {
      client_array[index].chatting = -1;
      char response[BUFFER_SIZE] = {0};
      strcpy(response, "User not found");
      encapsulate_nack(response);
      int val = send(*client_socket, response, BUFFER_SIZE, 0);
      manage_disconnect(client_array, index, *client_socket);
      if (val < 0) {
        printf("Message not sent...\n");
      }
      return;
    }
    unsigned long start_time = millis();   // Record the start time
    unsigned long duration = TIMEUOT_CONN; // Set the timer duration
    int flag = 1;
    for (;;) {
      if (client_array[input_from_client].chatting == index) {
        printf("Found a chat to create %d and %d\n", index, input_from_client);
        SocketPair *sockets = (SocketPair *)malloc(sizeof(SocketPair));
        sockets->client_1 = client_array[index].socket;
        sockets->client_2 = client_array[input_from_client].socket;
        manage_chat((void *)sockets, client_array, index);
        client_array[index].chatting = -1;
        client_array[input_from_client].chatting = -1;
        break;
      }
      if (millis() - start_time >= duration) {
        start_time = millis(); // Reset the start time for the next interval
        client_array[index].chatting = -1;
        char response[BUFFER_SIZE] = {0};
        strcpy(response,
               "Time expired, the other user did not connect with you...");
        encapsulate_nack(response);
        send(*client_socket, response, BUFFER_SIZE, 0);
        manage_disconnect(client_array, index, *client_socket);
        return;
      }
      if (flag == 1) {
        char response[BUFFER_SIZE] = {0};
        strcpy(response, "Wating for the other client to select you...\n");
        encapsulate_message(response);
        send(*client_socket, response, BUFFER_SIZE, 0);
        flag = 0;
      }
    }
  }
}

/*
 * This function manages the message
 *
 * Input:
 *    - char *body: The body of the message
 *    - int client_socket: The socket of the client
 *    - int client_socket_2: The socket of the other client
 * */
void manage_message(char *body, int client_socket, int client_socket_2) {
  char response[BUFFER_SIZE];
  strcpy(response, body);
  encapsulate_message(response);
  if ((send(client_socket_2, response, sizeof(response), 0)) < 0) {
    printf("Message not sent...\n");
    bzero(response, BUFFER_SIZE);
    strcpy(response, "Other client not in chat, please exit...");
    encapsulate_exit(response);
    if ((send(client_socket, response, sizeof(response), 0)) < 0) {
      printf("Message not sent...\n");
    }
  } else {
    bzero(response, BUFFER_SIZE);
    strcpy(response, "1");
    encapsulate_ack(response);
    if ((send(client_socket, response, sizeof(response), 0)) < 0) {
      printf("Message not sent...\n");
    }
  }
}

/*
 * This function manages the exit
 *
 * Input:
 *    - int client_socket_2: The socket of the other client
 * */
void manage_exit(int client_socket_2) {
  char response[BUFFER_SIZE];
  strcpy(response, "The other client has exited the chat, please exit...");
  encapsulate_exit(response);
  if ((send(client_socket_2, response, sizeof(response), 0)) < 0) {
    printf("Message not sent...\n");
  }
}

/*
 * This function creates the connection with the server
 *
 * Use on the client side
 *
 * Input:
 *    - int *client_socket: The socket of the client
 * */
void connect_to_server(int *client_socket) {
  char hostname[sizeof(HOSTNAME)];
  strcpy(hostname, HOSTNAME);

  struct sockaddr_in servaddr;

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

/*
 * This function initializes the connection for the server
 *
 * Use on the server side
 *
 * Input:
 *    - int *server_socket: The socket of the server
 * */
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

/*
 * This function accepts the connection with the client
 *
 * Use on the server side
 *
 * Input:
 *    - int *server_socket: The socket of the server
 *    - int *client_socket: The socket of the client
 * */
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

/*
 * This function analyzes the header of the message
 *
 * Use on the server side
 *
 * Input:
 *    - char *header: The header of the message. Mandatory
 *    - char *body: The body of the message. Mandatory
 *    - client *client_array: The array of clients. If no need send NULL
 *    - int index: The index of the client. If no need send -1
 *    - int client_socket: The socket of the client. Mandatory
 *    - int client_socket_2: The socket of the other client. If no need use -1
 *
 * Output:
 *    - int: The status of the message (0: success, -1: exit/disconnect)
 * */
int analyze_header_server(char *header, char *body, client *client_array,
                          int index, int client_socket, int client_socket_2) {
  if ((strcmp(header, "REGISTER") == 0)) {
    manage_register(body, client_array, index, client_socket);
    printf("-----registered----- \n");
  } else if ((strcmp(header, "CONNECT") == 0)) {
    manage_connect(body, client_array, index, &client_socket);
  } else if ((strcmp(header, "MESSAGE") == 0)) {
    manage_message(body, client_socket, client_socket_2);
  } else if ((strcmp(header, "EXIT") == 0)) {
    manage_exit(client_socket_2);
    return -1;
  } else if ((strcmp(header, "DISCONNECT") == 0)) {
    manage_disconnect(client_array, index, client_socket);
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
/*
 * This function uncapsulates the message reciived and analizes it
 *
 * Input:
 *    - char *buff: The message received. Mandatory
 *    - client *client_array: The array of clients. If no need send NULL
 *    - int index: The index of the client. If no need send -1
 *    - int client_socket: The socket of the client. Mandatory
 *    - int client_socket_2: The socket of the other client. if no need send -1
 *
 * Output:
 *    - int: The status of the message (0: success, -1: exit/disconnect)
 * */
int uncapsulate_server(char *buff, client *client_array, int index,
                       int client_socket, int client_socket_2) {
  char header[BUFFER_SIZE_HEADER], body[MAX_LEN_USERNAME * BACKLOG];

    char *token = strtok(buff, " "); // HEADER
    strcpy(header, token);  

    char *body_start = buff + strlen(header) + 1;  // body
    strcpy(body, body_start);  

    body[strcspn(body, "\n")] = '\0';

    return analyze_header_server(header, body, client_array, index, client_socket,
                               client_socket_2);
}

/*
 * This function reads the socket
 *
 * Input:
 *    - int client_socket: The socket of the client. Mandatory
 *    - char *buff: The buffer to store the message. Mandatory
 *    - client *client_array: The array of clients. If no need send NULL
 *    - int index: The index of the client. If no need send -1
 *
 * Output:
 *    - int: The status of the message (0: success, -1: exit/disconnect)
 * */
int read_socket(int client_socket, char *buff, client *client_array,
                int index) {
  int recv_len = recv(client_socket, buff, BUFFER_SIZE, 0);
  if (recv_len == 0) {
    printf("Client disconnected with socket number: %d\n", client_socket);
    if (client_array != NULL && index != -1) { // Just eliminate from server
      manage_disconnect(client_array, index, client_socket);
      printf("Remaining clients:\n");
      for (int i = 0; i < BACKLOG; ++i) {
        if (client_array[i].username != NULL) {
          printf("Client %d: %s\n", i + 1, client_array[i].username);
        }
      }
    }

    return -1;
  }
  if (recv_len < 0) {
    perror("recv failed...\n");
    if (client_array != NULL && index != -1) { // Just eliminate from server
      manage_disconnect(client_array, index, client_socket);
      printf("Remaining clients:\n");
      for (int i = 0; i < BACKLOG; ++i) {
        if (client_array[i].username != NULL) {
          printf("Client %d: %s\n", i + 1, client_array[i].username);
        }
      }
    }
    return -1;
  };
  return recv_len;
}

void encapsulate_register(char *msg) {
      char *locale = setlocale(LC_ALL, "");
    
    printf("Locale actual: %s\n", locale);
    
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

/*
 * This function analizes the header of the message
 *
 * Use on client side
 *
 * Input:
 *    - char *buffer: The message received. Mandatory
 *    - char *header: The header of the message. Mandatory
 *    - char *body: The body of the message. Mandatory
 *
 * Output:
 *    - int: The status of the message (0: success, -1: exit/disconnect)
 * */
int analize_header_client(char *buffer, char *header, char *body) {
  if ((strcmp(header, "ACK")) == 0) {
    printf(GREEN "󱅡" RESET);
    printf("\n");
  }
  if ((strcmp(header, "REGISTER")) == 0) { // ask to wich user to connect
    printf("\033[2J\033[1;1H");
    printf(CYAN "Type 'disconnect' in a chat to end the program\n");
    printf("Type 'exit' in a chat to exit that chat\n" RESET);
    printf("Type the %s index %s of the user you want to chat with\n", YELLOW,
           RESET);
    printf("Users available to connect: \n");
    printf("-1 to refresh: \n");
    printf("\n%s\n", body);
    bzero((void *)buffer, sizeof(buffer));
    printf("Connect with: \n");
  } else if ((strcmp(header, "NACK")) == 0) {
    printf("%s\n", body);
    printf(RED "NACK recieved\n" RESET);
    printf(RED "Send a key to RESTART..." RESET);
    printf("\n");
    return -1;
  } else if ((strcmp(header, "MESSAGE") == 0)) { // ask for message
    if (strcmp(body, "connected") == 0)
      printf("\033[2J\033[1;1H"); // when connect clear the console
    printf(BLUE ">>> " RESET);
    printf("%s\n", body); // print message from the other user
  } else if ((strcmp(header, "DISCONNECT") == 0)) {
    printf(YELLOW "Disconnecting chat with client\n");
    printf("Send a key to RESTART...\n" RESET);
    return -1;
  } else if ((strcmp(header, "EXIT") == 0)) {
    printf(YELLOW ">>> ");
    printf("%s" RESET, body); // print message from the other user
    printf("\n");
  }
  return 0;
}

/*
 * This function uncapsulates the message
 *
 * Use on client side
 *
 * Input:
 *    - char *buff: The message received. Mandatory
 *    - char *header: The header of the message. Mandatory
 *    - char *body: The body of the message. Mandatory
 *
 * Output:
 *    - int: The status of the message (0: success, -1: exit/disconnect)
 * */
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
