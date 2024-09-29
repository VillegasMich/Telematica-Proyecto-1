#include "mjep.h"
#include <stdio.h>
#include <string.h>

void connect_to_server() { printf("Connecting to server\n"); }

void create_conenction() { printf("Creating connection\n"); }

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
