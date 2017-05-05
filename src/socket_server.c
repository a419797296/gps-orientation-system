#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <netdb.h>
#include <errno.h>
#include <limits.h>
#include "socket_server.h"
#include "socket_driver.h"
#include "com_tools.h"
#include "main.h"

SOCKET_INTERFACE server_iface; 

// int server_iface.fd;
// pthread_t socket_server_threadID;
/***********local fucntion***********/
static void *server_thread(void *arg);
static void *client_thread(void *arg);
static void server_cleanup(void *arg);
static int server_init(void);

int server_init(void)
{
    char port[6]={0};
    server_iface.ip=malloc(16);
    memset(server_iface.ip,0,16);
    getSysUciCfg("spotInspection","server","port",port);
    server_iface.port = atoi(port);
    printf("port:%d\n",server_iface.port);
    return 0;
}
/******************************************************************************
Description.: this will stop the server thread, client threads
              will not get cleaned properly, because they run detached and
              no pointer is kept. This is not a huge issue, because this
              funtion is intended to clean up the biggest mess on shutdown.
Input Value.: id determines which server instance to send commands to
Return Value: always 0
******************************************************************************/
int server_stop(pthread_t threadID)
{

    printf("will cancel server thread #%02d\n", (int)threadID);
    pthread_cancel(threadID);

    return 0;
}

/******************************************************************************
Description.: This creates and starts the server thread
Input Value.: id determines which server instance to send commands to
Return Value: always 0
******************************************************************************/
int server_run(pthread_t *threadID)
{
    server_init();
    pthread_create(threadID, NULL, server_thread, (void *)&server_iface);
    printf("launching server thread #%02d\n", (int)(*threadID));
    pthread_detach(*threadID);

    return 0;
}

static void *server_thread(void *arg)
{
 int server_fd;
 pthread_t client;
     /* set cleanup handler to cleanup ressources */
    pthread_cleanup_push(server_cleanup, "test");
 server_fd = socketInit(server_iface.port);
 system("/root/led.sh blink_fast tp-link:blue:system");	//light on the led
 if (server_fd < 0)
 {
     printf("socket server init error\n");
     exit(1);
 }

  /* create a child for every client that connects */
  while(1) 
  {
    server_iface.fd = socketServerAccept(server_fd);
    if (server_iface.fd < 0)
        exit(1);
    sendProductInfo(server_iface.fd); 
    DBG("\n------------------------client is connected------------------------\n");  

    if(pthread_create(&client, NULL, &client_thread, (void *)&server_iface) != 0)
    {
        DBG("could not launch another client thread\n");
    }
    pthread_detach(client);
  }
  printf("leaving server thread, calling cleanup function now\n");
  pthread_cleanup_pop(1);
   return NULL;
}


/******************************************************************************
Description.: Serve a connected TCP-client. This thread function is called
              for each connect of a HTTP client like a webbrowser. It determines
              if it is a valid HTTP request and dispatches between the different
              response options.
Input Value.: arg is the filedescriptor and server-context of the connected TCP
              socket. It must have been allocated so it is freeable by this
              thread function.
Return Value: always NULL
******************************************************************************/
/* thread for clients that connected to this server */
static void *client_thread(void *arg)
{
  printf("-----------------this is the first step--------------");
  char buff[1024]={0};
  int nbyte;

    while(1)
    {
      if((nbyte=read(server_iface.fd,buff,1024))<=0)
      {
        printf("read err---------------\n");
        close(server_iface.fd);
        return NULL;
      }  
      buff[nbyte] = 0;
      printf("nbyte is %d,read data is %s\n",nbyte,buff);
      pthread_mutex_lock(&db);
    
printf("444444\n");
      memcpy(t_data_info.data, buff, nbyte+1);
printf("6666\n");
      t_data_info.length = nbyte;
      t_data_info.orig_fd = server_iface.fd;
printf("5555555\n");
      pthread_cond_broadcast(&db_update);// 发出一个数据更新的信号，通知发送通道来取数据
      pthread_mutex_unlock( &db );// 原子操作结束

    }
}
/******************************************************************************
Description.: This function cleans up ressources allocated by the server_thread
Input Value.: arg is not used
Return Value: -
******************************************************************************/
static void server_cleanup(void *arg)
{
    int i;

    printf("cleaning up ressources allocated by server thread: %s\n",(char *)arg);

    for(i = 0; i < MAX_SD_LEN; i++)
        //close(pcontext->sd[i]);
      printf("this is clean function %d\n", i);
}



// //     int on;
// //     pthread_t client;
// //     struct addrinfo *aip, *aip2;
// //     struct addrinfo hints;
// //     struct sockaddr_storage client_addr;
// //     socklen_t addr_len = sizeof(struct sockaddr_storage);
// //     fd_set selectfds;
// //     int max_fds = 0;
// //     char name[200]={0};
// //     int err;
// //     int i;   

// //     context *pcontext = arg;
// //     /* set cleanup handler to cleanup ressources */
// //     pthread_cleanup_push(server_cleanup, "test");

// //     printf("the parameter is ? %d\n",(int)arg);

// // printf("test postion 1\n");

// //     bzero(&hints, sizeof(hints));
// //     hints.ai_family = PF_UNSPEC;
// //     hints.ai_flags = AI_PASSIVE;
// //     hints.ai_socktype = SOCK_STREAM;

// // printf("test postion 2\n");
// // printf("pcontext->conf.port is %d \n",pcontext->conf.port); 

// // printf("test postion 3"); 
// //     snprintf(name, sizeof(name), "%d", ntohs(pcontext->conf.port));
// //  printf("test postion 4");    
// //     if((err = getaddrinfo(NULL, name, &hints, &aip)) != 0) {
// //         perror(gai_strerror(err));
// //         exit(EXIT_FAILURE);
// //     }

// // printf("after getaddrinfo, the name is %s\n",name); 
// //     for(i = 0; i < MAX_SD_LEN; i++)
// //         pcontext->sd[i] = -1;

// //  printf("test postion 3");


// //     /* open sockets for server (1 socket / address family) */
// //     i = 0;
// //     for(aip2 = aip; aip2 != NULL; aip2 = aip2->ai_next) {
// //         if((pcontext->sd[i] = socket(aip2->ai_family, aip2->ai_socktype, 0)) < 0) {
// //              printf("socket init erro \n");
// //             continue;
// //         }
// // printf("---------how many times \n");
// //         /* ignore "socket already in use" errors */
// //         on = 1;
// //         if(setsockopt(pcontext->sd[i], SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
// //             perror("setsockopt(SO_REUSEADDR) failed");
// //         }

// //         /* IPv6 socket should listen to IPv6 only, otherwise we will get "socket already in use" */
// //         on = 1;
// //         if(aip2->ai_family == AF_INET6 && setsockopt(pcontext->sd[i], IPPROTO_IPV6, IPV6_V6ONLY,
// //                 (const void *)&on , sizeof(on)) < 0) {
// //             perror("setsockopt(IPV6_V6ONLY) failed");
// //         }

// //         /* perhaps we will use this keep-alive feature oneday */
// //         /* setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on)); */

// //         if(bind(pcontext->sd[i], aip2->ai_addr, aip2->ai_addrlen) < 0) {
// //             perror("bind");
// //             pcontext->sd[i] = -1;
// //             continue;
// //         }

// //         if(listen(pcontext->sd[i], 10) < 0) {
// //             perror("listen");
// //             pcontext->sd[i] = -1;
// //         } else {
// //             i++;
// //             if(i >= MAX_SD_LEN) {
// //                 OPRINT("%s(): maximum number of server sockets exceeded", __FUNCTION__);
// //                 i--;
// //                 break;
// //             }
// //         }
// //     }

// //     pcontext->sd_len = i;

// //     if(pcontext->sd_len < 1) {
// //         OPRINT("%s(): bind(%d) failed", __FUNCTION__, htons(pcontext->conf.port));
// //         closelog();
// //         exit(EXIT_FAILURE);
// //     }
//      int server_fd,client_fd;
//      server_fd = socketServerInitNoneBlock(3333);
//      if (server_fd < 0)
//      {
//          printf("socket server init error\n");
//          return -1;
//      }
//      client_fd = socketServerAccept(socket_server_interface.server_fd);
//     if (client_fd < 0)
//         return -1;
//     DBG("\n------------------------client is connected------------------------\n");
    
//     /* create a child for every client that connects */
//     while(1) {
//         //int *pfd = (int *)malloc(sizeof(int));
//         cfd *pcfd = malloc(sizeof(cfd));

//         if(pcfd == NULL) {
//             fprintf(stderr, "failed to allocate (a very small amount of) memory\n");
//             exit(EXIT_FAILURE);
//         }

//         DBG("waiting for clients to connect\n");

//         do {
//             FD_ZERO(&selectfds);

//             for(i = 0; i < MAX_SD_LEN; i++) {
// DBG("pcontext->sd[i] %d\n",pcontext->sd[i]);
//                 if(pcontext->sd[i] != -1) {
//                     FD_SET(pcontext->sd[i], &selectfds);

//                     if(pcontext->sd[i] > max_fds)
//                         max_fds = pcontext->sd[i];
//                 }
//             }
// DBG("max_fds is %d\n",max_fds);
//             err = select(max_fds + 1, &selectfds, NULL, NULL, NULL);

//             if(err < 0 && errno != EINTR) {
//                 perror("select");
//                 exit(EXIT_FAILURE);
//             }
//         } while(err <= 0);
// DBG("waiting for clients to connect11111111%d\n",max_fds);

//         for(i = 0; i < max_fds + 1; i++) {
//             if(pcontext->sd[i] != -1 && FD_ISSET(pcontext->sd[i], &selectfds)) {
//                 pcfd->fd = accept(pcontext->sd[i], (struct sockaddr *)&client_addr, &addr_len);
//                 pcfd->pc = pcontext;

//                 /* start new thread that will handle this TCP connected client */
//                 DBG("create thread to handle client that just established a connection\n");

// #if 0
// /* commented out as it fills up syslog with many redundant entries */

//                 if(getnameinfo((struct sockaddr *)&client_addr, addr_len, name, sizeof(name), NULL, 0, NI_NUMERICHOST) == 0) {
//                     syslog(LOG_INFO, "serving client: %s\n", name);
//                 }
// #endif
//                 if(pthread_create(&client, NULL, &client_thread, pcfd) != 0) {
//                     DBG("could not launch another client thread\n");
//                     close(pcfd->fd);
//                     free(pcfd);
//                     continue;
//                 }
//                 pthread_detach(client);
//             }
//         }
//     }
     
//     printf("leaving server thread, calling cleanup function now\n");
//     pthread_cleanup_pop(1);


