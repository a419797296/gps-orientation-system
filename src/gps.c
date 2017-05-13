#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include "cJSON.h"
#include "socket_client.h"
#include "main.h"
#include "gps.h"
#include "socket_driver.h"
#include "com_tools.h"
#include "socket_ser2net.h"


//------------------------------------------------
void gpsUpdata(void)
{


}


//------------------------------------------------
void gpsInitSigaction(void)
{
    struct sigaction tact;
    tact.sa_handler = gpsTimer;
    tact.sa_flags = 0;
    sigemptyset(&tact.sa_mask);
    sigaction(SIGALRM, &tact, NULL);
}

//------------------------------------------------
void gpsTimer(int sig)
{
    DBG("It's the time to report .......\n");
    is_time_to_report = 1;
#ifdef RESPONSE
	char oxygen_ask_cmd[4]={0x11,0x01,0x01,0xED};
	socketWriteNoEnd(ser2net_iface.fd, oxygen_ask_cmd, 4);
#endif
}
//------------------------------------------------
void gpsInitTime(long ms)
{
    struct itimerval value;
    value.it_value.tv_sec = ms / 1000;
    value.it_value.tv_usec = ms % 1000 * 1000;
    value.it_interval = value.it_value;
	DBG("-------system will set the timeinterval to %ld-----\n",value.it_value.tv_sec);
    setitimer(ITIMER_REAL, &value, NULL);
}
//------------------------------------------------
void gpsRun(void)
{
	char time_interval[6]={0};
	getSysUciCfg("spotInspection","para","interval",time_interval);
	
	gpsInitSigaction();
    	gpsInitTime(atoi(time_interval)*1000);
}
//------------------------------------------------
void gpsCfgTm(PT_Data_Info pt_data_info)
{
    char *receivedData = pt_data_info->data;
    cJSON *json;
    int tmInterval;
    json=cJSON_Parse(receivedData);
    tmInterval = cJSON_GetObjectItem(json,"timeInterval")->valueint;
    gpsInitTime(tmInterval*1000);
	setSysUciCfgNum("spotInspection","para","interval",tmInterval);
    cJSON_Delete(json);
}

//------------------------------------------------
void gpsCfgServer(PT_Data_Info pt_data_info)
{
    char *receivedData = pt_data_info->data;
    cJSON *json;
    int port;
	char *ip;
    json=cJSON_Parse(receivedData);
    port = cJSON_GetObjectItem(json,"port")->valueint;
	ip = cJSON_GetObjectItem(json,"ip")->valuestring;
	setSysUciCfgNum("spotInspection","cloud","port",port);
	setSysUciCfgStr("spotInspection","cloud","ip",ip);
    cJSON_Delete(json);
}

//------------------------------------------------
void gpsReport(PT_Data_Info pt_data_info)
{
	char data[7] = {0};
	char time[7] = {0};
	uint16 short_addr=0;
    cJSON *root;
    char *out,*print_out;
    char *recieveData = pt_data_info->data;
	P_GpsAndSensOutData gps_sens_data=(P_GpsAndSensOutData)(recieveData+6);
	if(gps_sens_data->fix==0) 
		return;
	if(gps_sens_data->fix>0)
		{
		sprintf(data, "%02d%02d%02d",gps_sens_data->UTC_time.year,gps_sens_data->UTC_time.month,gps_sens_data->UTC_time.day);
		sprintf(time, "%02d%02d%02d",gps_sens_data->UTC_time.hour,gps_sens_data->UTC_time.min,gps_sens_data->UTC_time.sec);
		short_addr = BUILD_UINT16(*(recieveData+2), *(recieveData+3));
		    root=cJSON_CreateObject();
		    cJSON_AddNumberToObject(root, "jsonType", JSON_TYPE_GPS_REPOART);
		    cJSON_AddStringToObject(root, "productMac", produc_info.mac);
		    cJSON_AddNumberToObject(root, "shortAddr", short_addr);
			cJSON_AddStringToObject(root, "data", data);
			cJSON_AddStringToObject(root, "time", time);
			cJSON_AddNumberToObject(root, "location", gps_sens_data->location);
			printf("\n%x,%x,%x,%x",BREAK_UINT32( gps_sens_data->longitude, 3 ) ,BREAK_UINT32( gps_sens_data->longitude, 2 ),BREAK_UINT32( gps_sens_data->longitude, 1 ),BREAK_UINT32( gps_sens_data->longitude, 0 ));
			printf("\nshort int len=%d,int len=%d,long len= %d\n",sizeof(short),sizeof(int),sizeof(long));
			short test=21930;
			printf("high byte is %x, low byte is %x",*(uint8 *)&test,*(uint8 *)(&test+1));
			cJSON_AddNumberToObject(root, "longitude", (gps_sens_data->longitude)/10000000.0);
		    cJSON_AddNumberToObject(root, "latitude", (gps_sens_data->latitude)/10000000.0);
			cJSON_AddNumberToObject(root, "speed", gps_sens_data->speed);
			cJSON_AddNumberToObject(root, "direction", gps_sens_data->direction);
			cJSON_AddNumberToObject(root, "altitude", gps_sens_data->altitude);
			cJSON_AddNumberToObject(root, "temp", gps_sens_data->temp);
			cJSON_AddNumberToObject(root, "humi", gps_sens_data->humi);
			print_out=cJSON_Print(root);
			printf(print_out);
			free(print_out);
			out=cJSON_PrintUnformatted(root);
	}
	//------------------resolve the data

   

//------------------------if network is closed, try connect again and  save the data	
    if(socketWrite(cloud_iface.fd, out, strlen(out)) == -1)  //send data success
    		{
		cloud_iface.fd = -1;
		cloud_iface.fd = socketConnect(cloud_iface.ip,cloud_iface.port); 

		    if (cloud_iface.fd == -1)
		    {
		        printf("----------------can not reconnect to the server-------------\n");
	//------------------------here will add the data saving function			
		        return ;
		    }
		else
			{
			    printf("----------------have reconnect to the server-------------\n");
				system("/root/led.sh led_on tp-link:blue:system");	//light on the led
				sendProductInfo(cloud_iface.fd);
				
	//------------------------here will add the function of sending the resaved data
		}
	}
	else
		is_time_to_report = 0;
    
    cJSON_Delete(root);
    free(out);
//---------------------reset the time_interval time
	//char time_interval[6]={0};
	//getSysUciCfg("spotInspection","para","interval",time_interval);
    	//gpsInitTime(atoi(time_interval)*1000);
}


int gpsPackage(char *conbined_buf, int *conbined_len, char *new_data, int new_data_len, char first_byte, int max_pkg_len)
{
	DBG("------%s,data len is %d,------\n",new_data,*conbined_len);

	memcpy(conbined_buf + *conbined_len, new_data, new_data_len);
	*conbined_len=*conbined_len+new_data_len;
	if(conbined_buf[0]!=first_byte)
	{
		*conbined_len=0;
		DBG("can not match the first byte, %02x",conbined_buf[0]);
		return 0;
	}	
	if(*conbined_len>=max_pkg_len)
		{
		  int i=0;
	        unsigned char xor=0;
	        for(i=0; i<*conbined_len; i++)
	        {
	            DBG("%02x ",(unsigned char)conbined_buf[i]);
	            if(i<11)
	                xor-=conbined_buf[i];
	        }
	        DBG("\nthe xor is %02x \n",xor);
	        if(xor==(unsigned char)conbined_buf[11])
	        {
				*conbined_len = max_pkg_len;
				return 1;
	        }
	        else
	        {
	            *conbined_len = 0;
				return 0;
	        }
	}
	return 0;
}



