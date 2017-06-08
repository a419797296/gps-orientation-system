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
#include "socket_ser2net.h"
#include "socket_driver.h"
#include "com_tools.h"
#include "main.h"
int  allNum=0;
SOCKET_INTERFACE ser2net_iface; 
// int ser2net_iface.fd;
// pthread_t socket_ser2net_threadID;
/***********local fucntion***********/
static void *ser2net_thread(void *arg);
static void ser2net_cleanup(void *arg);
static int ser2net_init(void);

int ser2net_init(void)
{
    char port[6]={0};
    ser2net_iface.ip=malloc(16);
    memset(ser2net_iface.ip,0,16);
    getSysUciCfg("spotInspection","ser2net","ip",ser2net_iface.ip);
    getSysUciCfg("spotInspection","ser2net","port",port);
    ser2net_iface.port = atoi(port);
    ser2net_iface.fd = -1;
    printf("IP:%s,port:%d\n",ser2net_iface.ip, ser2net_iface.port);
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
int ser2net_stop(pthread_t threadID)
{

    printf("will cancel ser2net thread #%02d\n", (int)threadID);
    pthread_cancel(threadID);

    return 0;
}

/******************************************************************************
Description.: This creates and starts the server thread
Input Value.: id determines which server instance to send commands to
Return Value: always 0
******************************************************************************/
int ser2net_run(pthread_t *threadID)
{
    ser2net_init();
    pthread_create(threadID, NULL, ser2net_thread, (void *)NULL);
    printf("launching ser2net thread #%02d\n", (int)(*threadID));
    pthread_detach(*threadID);

    return 0;
}

static void *ser2net_thread(void *arg)
{
    char buff[1024]={0};
    char package[SER2NET_PACK_NUMS];
    int  nbyte;

    bool fullPackaged = 0;
    pthread_cleanup_push(ser2net_cleanup, "test");
    ser2net_iface.fd = socketConnect(ser2net_iface.ip,ser2net_iface.port); 
	
	
    if (ser2net_iface.fd == -1)
    {
        printf("----------------can not connect to the ser2net server-------------\n");
        return NULL;
    }
    printf("----------------have connect to the ser2net server-------------\n");
    while(1)
    {
	  if((nbyte=read(ser2net_iface.fd,package,1024))<=0)
      {
        printf("ser2net read err---------------\n");
        close(ser2net_iface.fd);
	ser2net_iface.fd = -1;
        return NULL;
      }  
	//  allNum=0;
	fullPackaged=getDataPkgFromSerial(buff, &allNum,package, nbyte, 0x24, '\n', 33);
		
      if(fullPackaged)
      {
        fullPackaged = 0;
        buff[allNum] = 0;
        printf("allNum is %d,read data is %s\n",allNum,buff);
	//if(is_time_to_report)
		{
	        pthread_mutex_lock(&db);
	        memcpy(t_data_info.data, buff, allNum+1);
	        t_data_info.length = allNum;
	        t_data_info.orig_fd = ser2net_iface.fd;
		
	        pthread_cond_broadcast(&db_update);// 发出一个数据更新的信号，通知发送通道来取数据
	        pthread_mutex_unlock( &db );// 原子操作结束
		}
		allNum = 0;
      }
    }

    pthread_cleanup_pop(1);
    return NULL;
}

/******************************************************************************
Description.: This function cleans up ressources allocated by the server_thread
Input Value.: arg is not used
Return Value: -
******************************************************************************/
static void ser2net_cleanup(void *arg)
{

    printf("cleaning up ressources allocated by ser2net thread: %s\n",(char *)arg);
}