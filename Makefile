ifeq ($(shell idl/genIf.sh ads idl),$(PWD))
endif
CC=g++
SHELL=/bin/sh
CURRENT_PATH=$(PWD)
INTERFACE_PATH=$(CURRENT_PATH)/idl/ads

INTERFACE_OBJ:=$(patsubst %.cpp,%.o, $(wildcard ${INTERFACE_PATH}/src/*.cpp))

INCLUDES=-I./include -I$(INTERFACE_PATH)/include -I./hashtable -I/usr/local/include/thrift/
LIB_PATH=-L. -L./lib -L/usr/local/lib/

CPPFLAGS=-O0 -g -static -fPIC -finline-functions -Winline -Werror -pipe \
        -Wreturn-type -Wtrigraphs -Wformat -Wparentheses -Wpointer-arith -Wall \
        --param large-function-growth=10000 --param inline-unit-growth=1000 \
        -D_XOPEN_SOURE=500 -D_GNU_SOURCE #-DTEST

LDFLAGS= -lm -lcrypt -lpthread -lgflags -lserv -lcap -llog \
		 -lini_config -lconn -lcluster -lzookeeper_mt \
		 -lthrift -lz

all: ads client_py_code 

test: testcmd ads_agent_test ads_cfg_test

%.o: %.cpp 
	$(CC) $(CPPFLAGS) $(INCLUDES) -c $< -o $@ 

ads: src/main.o src/ads_conf.o src/ads_conn.o src/ads_db_proc.o src/ads_init.o\
	src/ads_agent.o src/ads_agent_factory.o src/ads_agent_cfg.o src/db_layer_mgr.o\
	$(INTERFACE_OBJ)\
	src/ads_db_strategy.o src/hi_redis.o src/anet.o src/sds.o
	$(CC) -o $@ $^ $(INCLUDES) $(LIB_PATH) $(LDFLAGS) 

testcmd: test/redisTest.o\
	src/hi_redis.o src/anet.o src/sds.o
	$(CC) -o $@ $^ $(INCLUDES) $(LIB_PATH)

testclient:	test/client.o 
	$(CC) -o $@ $^ $(INCLUDES) $(LIB_PATH)

ads_agent_test: test/ads_agent_test.o \
	src/ads_agent.o src/ads_agent_factory.o
	$(CC) -o $@ $^ $(INCLUDES) $(LIB_PATH) $(LDFLAGS) 

ads_cfg_test: test/ads_cfg_test.o \
	src/ads_agent_cfg.o 
	$(CC) -o $@ $^ $(INCLUDES) $(LIB_PATH) $(LDFLAGS)

client_py_code:
	sh ./idl/gen.sh py
	/bin/rm -rf test/ads
	mv idl/gen-py/ads test/

clean:
	rm -rf src/*.o test/*.o ads ./test/ads ./idl/ads
	rm -rf testcmd

