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
#include "gps.h"

int socket_bussiness(void)
{


    while(1)
    {
        pthread_mutex_lock(&db);    
        pthread_cond_wait(&db_update, &db);

        printf("recieved the data is %s---------------\n",t_data_info.data);
        doit(&t_data_info);
		pthread_mutex_unlock( &db );// ԭ�Ӳ�������

    }
    return 0;
}

//--------------------------判断是否是json格式---------
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
        	json_type = JSON_TYPE_GPS_REPOART;
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

     case JSON_TYPE_GETWAY_TO_ZIGBEE:
         printf("jsonType is GETWAY_TO_ZIGBEE\r\n");
         socketWriteNoEnd(pt_data_info->dest_fd, pt_data_info->data, pt_data_info->length);   //+1 means sent '/0' together
         break;
     case JSON_TYPE_ZIGBEE_TO_GETWAY:
         printf("jsonType is ZIGBEE_TO_GETWAY\r\n");
	gpsReport(pt_data_info);
         // socketWriteNoEnd(pt_data_info->dest_fd, pt_data_info->data, pt_data_info->length);   //+1 means sent '/0' together
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
            write(pt_data_info->orig_fd,"connect",sizeof("connect"));
            printf("---------------connect-----\n");
            client_run(&cloud_iface.threadID);            
        }

        break;
	case JSON_TYPE_INTERVAL_CONFIG:
		printf("jsonType is JSON_TYPE_INTERVAL_CONFIG\r\n");
		//gpsCfgTm(pt_data_info);
	break;

	case JSON_TYPE_SERVER_CONFIG:
		printf("jsonType is JSON_TYPE_SERVER_CONFIG\r\n");
		//gpsCfgServer(pt_data_info);
	break;
	
	case JSON_TYPE_GPS_REPOART:
		//gpsReport(pt_data_info);
	break;
    default:
        printf("jsonType is default+\r\n");
        break;
    }
}

