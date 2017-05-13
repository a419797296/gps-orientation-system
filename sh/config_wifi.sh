#!/bin/sh

ssid_list_num=`uci get spotInspection.wifiInfo.ssid`
ssid=`uci get spotInspection.ssidList."ssid$ssid_list_num"`
key=`uci get spotInspection.wifiInfo.key`
#touch /root/"$ssid" /root/"$key"
#/root/wifi_start.sh sta $ssid $key

#--------------------------------------------------start sta
echo "starting sta mode"
ping www.baidu.com -c 1 -w 1 > nul

if [ $? == 0 ]
then
        SSID=`uci get wireless.sta.ssid`
        key=`uci get wireless.sta.key`
        echo the network have already connected,which SSID is $SSID, key is $key
	/root/led.sh blink_slow tp-link:blue:system
	spotInspection
        exit
fi

echo the input ssid is $ssid, key is $key
uci set wireless.sta.ssid=$ssid
uci set wireless.sta.key=$key
uci set wireless.sta.disabled=0
uci commit wireless
/etc/init.d/network restart

#-------------------------------------------------checking the wifi connection(if the key is not correct)

echo "checking sta mode"

        i=0
        while [ $i -ne 20 ]
        do
                ping www.baidu.com -c 1 -w 1 > nul
                if [ $? -eq 0 ]
                then
                        echo 'the network have already connected'
                        ip=`ifconfig wlan0|awk -F'[ :]+' '/inet addr/{print $4}'`
                        echo "the current ip is $ip"
			/root/led.sh blink_slow tp-link:blue:system
			spotInspection
                        exit
                fi
                i=$(($i+1))
                sleep 2
        done

#-------------------------------------------------if can not connect to the wifi, then change to the ap mode
        echo 'can not connect the wifi, changeing to the ap mode...'
	/root/led.sh blink_fast tp-link:blue:system
	uci set wireless.ap.encryption=none
	uci delete wireless.ap.key
	echo starting ap without key

uci set wireless.ap.ssid="Spot_Inspection"
uci set wireless.ap.disabled=0

#disable the sta mode
uci set wireless.sta.disabled=1

uci commit wireless
/etc/init.d/network restart




