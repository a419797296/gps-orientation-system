#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include "socket_driver.h"
#include "socket_client.h"
#include "socket_server.h"
#include "socket_ser2net.h"
#include "socket_bussiness.h"
//#include "ad7606_app.h"
#include "com_tools.h"
#include "cJSON.h"
#include "main.h"
#include "wifi_config.h"
#include "oxygen_flow.h"

int socket_bussiness(void)
{


    while(1)
    {
        pthread_mutex_lock(&db);    
        pthread_cond_wait(&db_update, &db);
	 /*if(check_connectiong(5) != 0)
  		system("/root/led.sh blink_fast tp-link:blue:system");	//flash the led if the network is not ready 
	if(!SocketConnected(cloud_iface.fd))
		{
		printf("have not connect to the server");
		if(check_connectiong(5) == 0)
			client_run(&cloud_iface.threadID);
		else
			printf("please check the network connection");
	}*/
		
        printf("recieved the data is %s---------------\n",t_data_info.data);
        doit(&t_data_info);
		pthread_mutex_unlock( &db );// 原子操作结束

    }
    return 0;
}

//--------------------------鍒ゆ柇鏄惁鏄痡son鏍煎紡---------
jsonType judgeJsonType(PT_Data_Info pt_data_info)
{
    cJSON *json;
    static jsonType json_type;
    char *receivedData = pt_data_info->data;
    int dataLength = pt_data_info->length;
    if(*receivedData=='{')
    {
        json=cJSON_Parse(receivedData);
        if (!json)
        {
            printf("Error before: [%s]\n",cJSON_GetErrorPtr());
        }
        else
        {
            json_type = cJSON_GetObjectItem(json,"jsonType")->valueint;
            cJSON_Delete(json);
        }
    }
    else if(*receivedData==0x16)
    	{
        	json_type = JSON_TYPE_OXYGEN_REPOART;
		pt_data_info->dest_fd = cloud_iface.fd;
    	}
    else
    {
        if (receivedData[0]=='$' && receivedData[1]=='@' && receivedData[dataLength-2]=='\r' && receivedData[dataLength-1]=='\n')
        {
            printf("------------------success+++--%d------------\n",dataLength);
		if(pt_data_info->orig_fd == cloud_iface.fd)
			{
			json_type = JSON_TYPE_GETWAY_TO_ZIGBEE;
			pt_data_info->dest_fd = ser2net_iface.fd;
		}
			
		if(pt_data_info->orig_fd == ser2net_iface.fd)
			{
			json_type = JSON_TYPE_ZIGBEE_TO_GETWAY;
			pt_data_info->dest_fd = cloud_iface.fd;
		}
            		
        }
        else
        {
            printf("------------------ERROR+++--%d------------\n",dataLength);
            json_type = JSON_TYPE_ERROR;
        }
    }
    return json_type;
}

/* Parse text to JSON, then render back to text, and print! */
void doit(PT_Data_Info pt_data_info)
{

    pt_data_info->type=judgeJsonType(pt_data_info);
    switch(pt_data_info->type)
    {
    case JSON_TYPE_ERROR:
        printf("jsonType is ERROR\r\n");
        break;
    // case JSON_TYPE_CONTROL_CMD:
    //     printf("jsonType is CONTROL_CMD\r\n");
    //     doControlInfo(receivedData);
    //     break;
     case JSON_TYPE_GETWAY_TO_ZIGBEE:
         printf("jsonType is GETWAY_TO_ZIGBEE\r\n");
         socketWriteNoEnd(ser2net_iface.fd, pt_data_info->data, pt_data_info->length);   //+1 means sent '/0' together
         break;
     case JSON_TYPE_ZIGBEE_TO_GETWAY:
         printf("jsonType is ZIGBEE_TO_GETWAY\r\n");
          socketWriteNoEnd(cloud_iface.fd, pt_data_info->data, pt_data_info->length);   //+1 means sent '/0' together
          break;

    case JSON_TYPE_WIFI_CONFIG:
        printf("jsonType is JSON_TYPE_WIFI_CONFIG\r\n");
        if (config_wifi(pt_data_info)==-1)
        {
            printf("---------------unconnect-----\n");
            write(pt_data_info->orig_fd,"unconnect",sizeof("unconnect"));
        }
        else
        {
            printf("---------------connect-----\n");
            write(pt_data_info->orig_fd,"connect",sizeof("connect"));
            client_run(&cloud_iface.threadID);            
        }

        break;
	case JSON_TYPE_INTERVAL_CONFIG:
		printf("jsonType is JSON_TYPE_INTERVAL_CONFIG\r\n");
		oxygenFlowCfgTm(pt_data_info);
	break;

	case JSON_TYPE_SERVER_CONFIG:
		printf("jsonType is JSON_TYPE_SERVER_CONFIG\r\n");
		oxygenFlowCfgServer(pt_data_info);
	break;
	
	case JSON_TYPE_OXYGEN_REPOART:
		oxygenFlowReport(pt_data_info);
	break;
    default:
        printf("jsonType is default+\r\n");
        break;
    }
}

