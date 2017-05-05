#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H
#include "main.h"

#define  MAX_SD_LEN   50
// extern int socket_server_fd;
// extern pthread_t socket_server_threadID;
extern SOCKET_INTERFACE server_iface; 
/* prototypes */
int server_stop(pthread_t threadID);
int server_run(pthread_t *threadID);


#endif