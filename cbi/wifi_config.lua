
luci.sys.exec("/root/scan_ssid.sh")
m = Map("spotInspection", "Network Configeration", "Please choose the SSID from the ssid list and enter the wifi key, then press the save&apply button to commit")
m:chain("luci")

s = m:section(TypedSection, "wifi_info", translate("Wifi Information"))
s.anonymous = true                                                                   
s.addremove = false 

s:tab("wifi", translate("Config Wifi"))

o = s:taboption("wifi", ListValue, "ssid", translate("SSID:")) 
--o.default = 0 
o.datatype = "uinteger"

max_ssid = luci.sys.exec("uci get spotInspection.ssidList.max_ssid")
for i=0,max_ssid,1
do
	str=string.format("uci get spotInspection.ssidList.ssid%d",i)
	ssid = luci.sys.exec(str)
	o:value(i, translate(ssid)) 
end


key = s:taboption("wifi", Value, "key", translate("Key:"),"input the wifi key") 

local apply = luci.http.formvalue("cbi.apply")
if apply then
	io.popen("/root/config_wifi.sh")
end

return m


