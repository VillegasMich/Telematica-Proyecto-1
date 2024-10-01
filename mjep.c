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
    //Toma el contenido del body y lo guarda en la estructura de datos
  } else if((strcmp(header, "CONNECT") == 0)){
    printf("CON\n");
    //Busca en la estructura de datos el body
    //Si lo encuentra crea la pareja de clientes
  }else if((strcmp(header, "MESSAGE") == 0)){
    printf("MESS\n");
    // Lee el body, lo encapsula y lo manda al otro cliente
  } else if((strcmp(header, "EXIT") == 0)){
    printf("EX\n");
    // Rompe la pareja de clientes
  } else if((strcmp(header, "DISCONNECT") == 0)){
    printf("DIS\n");
    // Mata al cliente
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
