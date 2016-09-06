MAIN=httpserver
CC=g++ 
CFLAGS=-Wall -Wno-deprecated -g #-O2 
BOOST_DIR = /usr/local/include

INCLUDE = -I$(BOOST_DIR) -I /usr/local/include/evhtp/ 

LIBS = -L/usr/local/lib/  -levent -lrt -lssl -lcrypto /usr/local/lib/libevhtp.a \
	   -levent -lssl -lcrypto -levent_openssl -lboost_thread -lboost_system -pthread

objects = ./src/server.o ./src/proxy.o ./src/workers.o ./src/simple_httphandler.o \
		  ./src/request_buffer.o 

all: server 

server: $(objects)
	$(CC) $(CFLAGS) $(INCLUDE) -o ./bin/${MAIN} $^ $(LIBS) 

$(objects): %.o: %.cc
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -rf src/*.o bin/*
