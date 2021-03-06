CC=~/bin/g++

INCLUDES=-I./include
LIB_PATH=-L. -L./lib

CPPFLAGS=-O0 -g -static -fPIC -finline-functions -Werror -Winline -pipe \
        -Wreturn-type -Wtrigraphs -Wformat -Wparentheses -Wpointer-arith -Wall \
        --param large-function-growth=10000 --param inline-unit-growth=1000 \
        -D_XOPEN_SOURE=500 -D_GNU_SOURCE -std=c++0x  

LDFLAGS=""

all: reflect

%.o: %.cpp 
	$(CC) $(CPPFLAGS) $(INCLUDES) -c $< -o $@ 

reflect: test/reflect.o test/class_define.o
	$(CC) -o $@ $^ $(INCLUDES) $(LIB_PATH)

clean:
	rm -rf src/*.o test/*.o reflect

