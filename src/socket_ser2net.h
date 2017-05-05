#ifndef SOCKET_SER2NET_H
#define SOCKET_SER2NET_H
#include "main.h"
// extern int socket_ser2net_fd;
// extern pthread_t socket_ser2net_threadID;
extern SOCKET_INTERFACE ser2net_iface; 
#define SER2NET_IP			"127.0.0.1"
#define	SER2NET_PORT		6666
#define	SER2NET_PACK_NUMS	128

//#define SER2NET_END_WITH_NULL			
#define SER2NET_END_WITH_LENGTH	

#ifdef SER2NET_END_WITH_LENGTH
#define SER2NET_FIX_LENGTH						12
#endif

/* prototypes */
int ser2net_stop(pthread_t threadID);
int ser2net_run(pthread_t *threadID);

#endif