#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "wifi_config.h"
#include "cJSON.h"


int config_wifi(PT_Data_Info pt_data_info)
{
    cJSON *json;
    char *ssid;
    char *key;
    char cmd_buff[100];
    //------------------获取参数----
    json=cJSON_Parse(pt_data_info->data);
    ssid = cJSON_GetObjectItem(json,"ssid")->valuestring;
    key = cJSON_GetObjectItem(json,"key")->valuestring;
    memset(cmd_buff,0,100);
    sprintf(cmd_buff,"/root/wifi_start_sta.sh %s %s",ssid,key);
    printf("the cmd is %s\n", cmd_buff);
    system(cmd_buff);
    cJSON_Delete(json);
    return check_connectiong(30);
}
//-----------------------check connect------------
int check_connectiong(int test_time)
{
  while(test_time--)
  {
    if (!system("ping 115.239.210.27 -c 1 -w 1 > nul"))
    {
      return 0;    //successed
    }
    printf("ping failed------------\n");
    sleep(1);
  }
  system("/root/wifi_start_ap.sh Spot_inspection none");  //if not successs, stat the ap mode
  return -1;
}