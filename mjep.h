#ifndef MJEP_H
#define MJEP_H
#define BUFFER_SIZE_MSG 100
#define BUFFER_SIZE_HEADER 15
#define BUFFER_SIZE BUFFER_SIZE_HEADER + BUFFER_SIZE_MSG

void connect_to_server(); // client
void create_conenction(); // server
void encapsulate_register(char msg[BUFFER_SIZE_MSG]);
void encapsulate_connect(char msg[BUFFER_SIZE_MSG]);
void encapsulate_message(char msg[BUFFER_SIZE_MSG]);
void encapsulate_exit(char msg[BUFFER_SIZE_MSG]);
void encapsulate_disconnect(char msg[BUFFER_SIZE_MSG]);
void uncapsulate_register(char msg[BUFFER_SIZE]);
void uncapsulate_connect(char msg[BUFFER_SIZE]);
void uncapsulate_message(char msg[BUFFER_SIZE]);
void uncapsulate_exit(char msg[BUFFER_SIZE]);
void uncapsulate_disconnect(char msg[BUFFER_SIZE]);

#endif
