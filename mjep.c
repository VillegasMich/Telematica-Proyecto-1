#include "mjep.h"



void connect_to_server() { printf("Connecting to server\n"); }

//initialize the server Socket - main
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
    if ((bind(*server_socket, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
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

void accept_connection(int *server_socket, int *client_socket){
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

void analyze_header(char *header, char *body) {
  if ((strcmp(header, "REGISTER") == 0)){
    printf("REG\n");
  } else if((strcmp(header, "CONNECT") == 0)){
    printf("CON\n");
  }else if((strcmp(header, "MESSAGE") == 0)){
    printf("MESS\n");
  } else if((strcmp(header, "EXIT") == 0)){
    printf("EX\n");
  } else if((strcmp(header, "DISCONNECT") == 0)){
    printf("DIS\n");
  }
}

void uncapsulate_msg(char *buff){
    char header[BUFFER_SIZE_HEADER], body[BUFFER_SIZE_MSG];
    //split the header and the body
    sscanf(buff, "%s %s", header, body);
    analyze_header(header, body);
}

void read_client(int *client_socket, char *buff){
    bzero((void *)buff, BUFFER_SIZE);
    recv(*client_socket, buff, BUFFER_SIZE, 0);
}

void encapsulate_register(char msg[BUFFER_SIZE_MSG]) {
  printf("Encapsulating register\n");
  printf("Message: %s\n", msg);
  char encapsulated_msg[BUFFER_SIZE_HEADER] = "REGISTER ";
  strcat(encapsulated_msg, msg);
  printf("Encapsulated message: %s\n", encapsulated_msg);
}

void encapsulate_connect(char msg[BUFFER_SIZE_MSG]) {
  printf("Encapsulating connect\n");
  printf("Message: %s\n", msg);
  char encapsulated_msg[BUFFER_SIZE_HEADER] = "CONNECT ";
  strcat(encapsulated_msg, msg);
  printf("Encapsulated message: %s\n", encapsulated_msg);
}

void encapsulate_message(char msg[BUFFER_SIZE_MSG]) {
  printf("Encapsulating message\n");
  printf("Message: %s\n", msg);
  char encapsulated_msg[BUFFER_SIZE_HEADER] = "MESSAGE ";
  strcat(encapsulated_msg, msg);
  printf("Encapsulated message: %s\n", encapsulated_msg);
}

void encapsulate_exit(char msg[BUFFER_SIZE_MSG]) {
  printf("Encapsulating exit\n");
  printf("Message: %s\n", msg);
  char encapsulated_msg[BUFFER_SIZE_HEADER] = "EXIT ";
  strcat(encapsulated_msg, msg);
  printf("Encapsulated message: %s\n", encapsulated_msg);
}

void encapsulate_disconnect(char msg[BUFFER_SIZE_MSG]) {
  printf("Encapsulating disconnect\n");
  printf("Message: %s\n", msg);
  char encapsulated_msg[BUFFER_SIZE_HEADER] = "DISCONNECT ";
  strcat(encapsulated_msg, msg);
  printf("Encapsulated message: %s\n", encapsulated_msg);
}

void uncapsulate_register(char msg[BUFFER_SIZE]) {
  printf("Uncapsulating register: %s\n", msg);
  char *ptr = msg;
  char *unencapsulated_msg = ptr + strlen("REGISTER ");
  printf("Unencapsulated message: %s\n", unencapsulated_msg);
}

void uncapsulate_connect(char msg[BUFFER_SIZE]) {
  printf("Uncapsulating connect: %s\n", msg);
  char *ptr = msg;
  char *unencapsulated_msg = ptr + strlen("CONNECT ");
  printf("Unencapsulated message: %s\n", unencapsulated_msg);
}

void uncapsulate_message(char msg[BUFFER_SIZE]) {
  printf("Uncapsulating message: %s\n", msg);
  char *ptr = msg;
  char *unencapsulated_msg = ptr + strlen("MESSAGE ");
  printf("Unencapsulated message: %s\n", unencapsulated_msg);
}

void uncapsulate_exit(char msg[BUFFER_SIZE]) {
  printf("Uncapsulating exit: %s\n", msg);
  char *ptr = msg;
  char *unencapsulated_msg = ptr + strlen("EXIT ");
  printf("Unencapsulated message: %s\n", unencapsulated_msg);
}

void uncapsulate_disconnect(char msg[BUFFER_SIZE]) {
  printf("Uncapsulating disconnect: %s\n", msg);
  char *ptr = msg;
  char *unencapsulated_msg = ptr + strlen("DISCONNECT ");
  printf("Unencapsulated message: %s\n", unencapsulated_msg);
}
