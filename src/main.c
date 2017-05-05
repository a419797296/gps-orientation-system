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
#include "socket_client.h"
#include "socket_driver.h"
#include "socket_ser2net.h"
#include "socket_bussiness.h"
#include "wifi_config.h"
#include "com_tools.h"

pthread_mutex_t db;
pthread_cond_t  db_update;
char *out_data;
bool is_time_to_report = 0;
PT_Data_Info pt_data_info;
T_Data_Info t_data_info;
PRODUCT_INFO produc_info;

//----------------------------------------
static void systemInit(void);


//----------------------------------------
void systemInit(void)
{
  //char macAddr[18] = {0};
  //char hw_vers[8] = {0};
  //char sw_vers[8] = {0};
  memset(produc_info.hw_vers, 0, sizeof(produc_info.hw_vers));
  memset(produc_info.sw_vers, 0, sizeof(produc_info.sw_vers));
  memset(produc_info.mac, 0, sizeof(produc_info.mac));
  if(getMacAddr("eth0",produc_info.mac) == -1)
  {
      printf("read the mac error: readed data is %s\n", produc_info.mac);
  }
  getSysUciCfg("spotInspection","product_info","hw_vers",produc_info.hw_vers);
  getSysUciCfg("spotInspection","product_info","sw_vers",produc_info.sw_vers);
  //produc_info.hw_vers = strdup(hw_vers);
  //produc_info.sw_vers = strdup(sw_vers);
  //produc_info.mac = strdup(macAddr);
  printf("produc_info.mac:%s,produc_info.hw_vers:%s,produc_info.sw_vers :%s\n",produc_info.mac,produc_info.hw_vers,produc_info.sw_vers );
  system("/root/led.sh blink_slow tp-link:blue:system");	//light on the led
   //system("/root/spotInspection_init.sh");	//init working environment

}
//----------------------------------------
int main(int argc,char *argv[])
{
  systemInit();
  char rst[20];
  memset(rst,0,20);
  //while(1)
   // sleep(1);

  if( pthread_mutex_init(&db, NULL) != 0 )
  /* 初始化 global.db 成员 */
    {
      return -1;
    }
    if( pthread_cond_init(&db_update, NULL) != 0 )
    /* 初始化 global.db_update(条件变量) 成员 */
    {
      printf("could not initialize condition variable\n");
      return -1;
    }
  memset(&t_data_info, 0, sizeof(t_data_info));
  t_data_info.data = malloc(1024);
  if (t_data_info.data ==NULL)
  {
    printf("malloc error\n");
  }

  ser2net_run(&ser2net_iface.threadID);
  if(check_connectiong(30) != 0)
  	system("/root/led.sh blink_fast tp-link:blue:system");	//flash the led if the network is not ready 
  client_run(&cloud_iface.threadID);
  socket_bussiness();
  return 0;
}