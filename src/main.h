#ifndef MAIN_H
#define MAIN_H

#define _DEBUG_TRACE_CMH_	2  

  
#if 1==_DEBUG_TRACE_CMH_    //普通打印  
    #define TRACE_CMH printf  
#elif 2==_DEBUG_TRACE_CMH_  //打印文件名、行号  
    #define TRACE_CMH(fmt,...)	printf("%s(%d): "##fmt, __FILE__, __LINE__, ##__VA_ARGS__)  
#elif 3==_DEBUG_TRACE_CMH_  //打印文件名、行号、函数名  
    #define TRACE_CMH(fmt,...)	printf("%s(%d)-<%s>: "##fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)  
#else  
    #define TRACE_CMH  
#endif //_TRACE_CMH_DEBUG_  

/* ------------------------------------------------------------------------------------------------
 *                                               Types
 * ------------------------------------------------------------------------------------------------
 */
typedef signed   char   int8;
typedef unsigned char   uint8;

typedef signed   short  int16;
typedef unsigned short  uint16;

typedef signed   long   int32;
typedef unsigned long   uint32;

typedef unsigned char   bool;

#define BUILD_UINT16(hiByte, loByte) \
          ((uint16)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

#define BUILD_UINT8(hiByte, loByte) \
          ((uint8)(((loByte) & 0x0F) + (((hiByte) & 0x0F) << 4)))

#define HI_UINT8(a) (((a) >> 4) & 0x0F)
#define LO_UINT8(a) ((a) & 0x0F)



#define MAX_CLIENT_NUM	5
typedef enum
{
	JSON_TYPE_GETWAY_TO_ZIGBEE = 1,
	JSON_TYPE_ZIGBEE_TO_GETWAY, 
	JSON_TYPE_CONTROL_CMD,
	JSON_TYPE_DATA_REPOART,
	JSON_TYPE_PRODUCT_INFO,
	JSON_TYPE_OXYGEN_REPOART,
	JSON_TYPE_WIFI_CONFIG,
	JSON_TYPE_INTERVAL_CONFIG,
	JSON_TYPE_SERVER_CONFIG,
	JSON_TYPE_ERROR
}jsonType;

 typedef struct{
	char		*ip;
	int			port;
	int 		fd;
	pthread_t 	threadID;
}SOCKET_INTERFACE;

 typedef struct{
	char		hw_vers[8];
	char		sw_vers[8];
	char 		mac[18];
}PRODUCT_INFO;

 typedef struct{
	char		*data;
	int			length;
	jsonType 	type;
	int 		orig_fd;
	int 		dest_fd;
}T_Data_Info, *PT_Data_Info;


extern PT_Data_Info pt_data_info;
extern T_Data_Info t_data_info;
extern PRODUCT_INFO produc_info;
extern bool is_time_to_report;
#define HARDWARE_VERSION				"V3.0"
#define	SOFTWARE_VERSION				"20170327"
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]) 

/*******************************************************/  
// #include <pthread.h>
// typedef struct Server {
//   int 				fd;
//   pthread 			threadID;
//   pthread_mutex_t 	db;
//   pthread_cond_t  	db_update;
// }T_Server, *PT_Servrer;

#define DEBUG

#ifndef DBG
#ifdef DEBUG
#define DBG(...) printf(__VA_ARGS__)
#else
#define DBG(...)
#endif
#endif

#define OUTPUT_PLUGIN_PREFIX " o: "
#define OPRINT(...) { char _bf[1024] = {0}; snprintf(_bf, sizeof(_bf)-1, __VA_ARGS__); fprintf(stderr, "%s", OUTPUT_PLUGIN_PREFIX); fprintf(stderr, "%s", _bf); syslog(LOG_INFO, "%s", _bf); }

extern pthread_mutex_t db;
extern pthread_cond_t  db_update;
extern char *out_data;


#endif