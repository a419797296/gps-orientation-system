#!/bin/sh

/root/get_mac.sh eth0
cat /etc/ser2net.conf | grep 6666
if [ $? -ne 0 ];then
	echo 6666:raw:0:/dev/ttyATH0:115200 NONE 1STOPBIT 8DATABITS   -RTSCTS >> /etc/ser2net.conf
	kill `pidof ser2net`
	ser2net
#else
#	echo "ser2net server has already started"
fi


pidof ser2net
if [ $? -ne 0 ];then
	ser2net
	echo "ser2net server has not started"
#else
#	echo "ser2net server has already started"
fi

/root/config_wifi.sh




