#!/bin/sh
n=0
iw dev wlan0 scan | grep SSID| awk -F'[:]' '{print $2}'|while read line
do
	echo here is :----$line----
	uci set spotInspection.ssidList."ssid$n"="$line"
	n=$(($n+1))
done
uci commit
