#添加本地文件
LOCAL_SRC=$(wildcard ./*.c)
LOCAL_OBJ=$(patsubst ./%.c, ./%.o, $(LOCAL_SRC))

#添加lib目录中的库文件
LIB_SRC=$(wildcard ../lib/*.c)
LIB_OBJ=$(patsubst ../lib/%.c, ../lib/%.o, $(LIB_SRC))

#添加bms目录中的设备文件
BMS_SRC=$(wildcard ../bms/*.c)
BMS_OBJ=$(patsubst ../bms/%.c, ../bms/%.o, $(BMS_SRC))

#添加pcs目录中的设备文件
PCS_SRC=$(wildcard ../pcs/*.c)
PCS_OBJ=$(patsubst ../pcs/%.c, ../pcs/%.o, $(PCS_SRC))

#添加dido目录中的设备文件
DIDO_SRC=$(wildcard ../dido/*.c)
DIDO_OBJ=$(patsubst ../dido/%.c, ../dido/%.o, $(DIDO_SRC))

#添加serival_device目录中的设备文件
DEV_SRC=$(wildcard ../serival_device/*.c)
DEV_OBJ=$(patsubst ../serival_device/%.c, ../serival_device/%.o, $(DEV_SRC))

#添加logic目录中的设备文件
LOGIC_SRC=$(wildcard ../logic/*.c)
LOGIC_OBJ=$(patsubst ../logic/%.c, ../logic/%.o, $(LOGIC_SRC))

#添加tcp_sever的代码
SERVER_SRC=$(wildcard ../tcp_server/*.c)
SERVER_OBJ=$(patsubst ../tcp_server/%.c, ../tcp_server/%.o, $(SERVER_SRC))

#添加JSON的代码
JSON_SRC=$(wildcard ../cJSON/*.c)
JSON_OBJ=$(patsubst ../cJSON/%.c, ../cJSON/%.o, $(JSON_SRC))

#配置
CC=arm-linux-gnueabihf-gcc
#CC=/usr/local/arm/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
INCLUDES = -I../base  -I../lib -I../bms -I../pcs -I../dido -I../serival_device -I../logic -I../tcp_server  -I../IEC104_include -I../cJSON 
LIBS += -L../lib -lpthread -lzlog -std=gnu99 -lm

#编译过程
base_tems:$(LOCAL_OBJ) $(LIB_OBJ)$(BMS_OBJ) $(PCS_OBJ) $(DIDO_OBJ) $(DEV_OBJ) $(LOGIC_OBJ) $(SERVER_OBJ) $(JSON_OBJ) 
	rm -rf base_tems
	$(CC) $(LIBS) $(INCLUDES) $(MACRO) -g -o $@ $^
	@rm $(LOCAL_OBJ) $(LIB_OBJ)$(BMS_OBJ) $(PCS_OBJ) $(DIDO_OBJ) $(DEV_OBJ) $(LOGIC_OBJ) $(SERVER_OBJ) $(JSON_OBJ) 
	#cp tems /nfsroot
	sudo mount -t nfs -o nolock,nfsvers=3 192.168.2.4:/home/moxa /home/normal/trina_code/lc_base/moxa/
	sudo cp base_tems ../moxa/
	sudo umount /home/normal/trina_code/lc_base/moxa/
%.o: %.c
	$(CC) $(LIBS) $(INCLUDES) $(MACRO) -g -c $^ -o $@

#命令
.PHONY: clean

clean:
	rm *.o ../lib/*.o ../bms/*.o ../pcs/*.o ../dido/*.o ../logic/*.o ../tcp_server/*.o ../pcs/*.o ../serival_device/*.o ../cJSON/*.o 
