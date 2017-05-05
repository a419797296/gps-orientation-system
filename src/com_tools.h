#ifndef COM_TOOLS_H_
#define COM_TOOLS_H_


int getMacAddr(char *device,char * macAddrBuff);
void sendProductInfo(int sockfd);		 
int SendStringSCIByPackage(int socket_fd,char * dataString,int packageSize,int delayTime); 
int  JsonResolveInt(char* dataString, char *str);
void StrToHex(char *pbDest, char *pbSrc, int nLen);
void HexToStr(char *pbDest, char *pbSrc, int nLen);
int pow_of_two(int num);
int getSysUciCfg(char *filename,char *section,char *option,char * result);
int setSysUciCfgStr(char *filename,char *section,char *option,char * parameter);
int setSysUciCfgNum(char *filename,char *section,char *option,int parameter);
char XorCheckSum(char * pBuf, char len);
int getDataPkgFromSerial(char *conbined_buf, int *conbined_len, char *new_data, int new_data_len, char first_byte, char end_byte, int max_pkg_len) ; //if end_byte ==0.means if recieved 0,then return;end_byte ==0xff, ignore the end byte;else,must match the end byte. 
#endif
