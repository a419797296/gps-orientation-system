.PHONY:clean

OBJECTS= main.o socket_server.o socket_client.o socket_ser2net.o wifi_config.o socket_driver.o socket_bussiness.o com_tools.o cJSON.o  gps.o

gps_oritation_app:$(OBJECTS)
	mips-openwrt-linux-gcc -Wall  $^ -o $@ -lm -lpthread -Wl,--rpath-link /opt/OpenWrt-Toolchain-ar71xx-for-mips_34kc-gcc-4.8-linaro_uClibc-0.9.33.2/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-0.9.33.2/lib
socket_client.o:socket_client.c
	mips-openwrt-linux-gcc -Wall  -c $< -o $@ -lpthread
socket_server.o:socket_server.c
	mips-openwrt-linux-gcc -Wall  -c $< -o $@ -lpthread
socket_ser2net.o:socket_ser2net.c
	mips-openwrt-linux-gcc -Wall  -c $< -o $@ -lpthread
socket_driver.o:socket_driver.c
	mips-openwrt-linux-gcc -Wall  -c $< -o $@
main.o:main.c
	mips-openwrt-linux-gcc -Wall  -c $< -o $@
wifi_config.o:wifi_config.c
	mips-openwrt-linux-gcc -Wall  -c $< -o $@
socket_bussiness.o:socket_bussiness.c
	mips-openwrt-linux-gcc -Wall  -c $< -o $@
com_tools.o:com_tools.c
	mips-openwrt-linux-gcc -Wall  -c $< -o $@
gps.o:gps.c
	mips-openwrt-linux-gcc -Wall  -c $< -o $@
cJSON.o:cJSON.c
	mips-openwrt-linux-gcc -Wall  -c $< -o $@ -lm
clean:
	rm -f $(OBJECTS) gps_oritation_app

