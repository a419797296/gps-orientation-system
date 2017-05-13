#ifndef GPS_H
#define GPS_H

#define REPORT_TIME_INTERVAL	2000   //ms

#define RESPONSE

void gpsInitSigaction(void);
void gpsInitTime(long ms);
void gpsTimer(int sig);
void gpsRun(void);
void gpsCfgTm(PT_Data_Info pt_data_info);
void gpsCfgServer(PT_Data_Info pt_data_info);
void gpsReport(PT_Data_Info pt_data_info);
void gpsUpdata(void);
int gpsPackage(char *conbined_buf, int *conbined_len, char *new_data, int new_data_len, char first_byte, int max_pkg_len);
typedef struct _time_
{
	uint8 year;
	uint8 month;
	uint8 day;
	uint8 hour;
	uint8 min;
	uint8 sec;
}TIME;

typedef struct {
  uint8 fix;  //0:未定位　1:2D定位　2:3D定位    //1 byte
  TIME UTC_time;                                //6 byte
  uint8 location;                                // //1 byte 0x11,东经，北纬
  uint32 longitude;				// //4 byte 
  uint32 latitude;				//byte
  uint16 speed;				//byte
  uint16 direction;                       // //2 byte
  uint16 altitude; 	                     //byte
  uint8 temp;                              // 1byte
  uint8 humi;					// 1 byte
}T_GpsAndSensOutData,*P_GpsAndSensOutData;
#endif