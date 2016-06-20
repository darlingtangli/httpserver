MAIN=httpserver
CLIENT=client
CC=g++ 
CFLAGS=-Wall -Wno-deprecated -g #-O2 
BOOST_DIR = /usr/local/include

INCLUDE = -I$(BOOST_DIR) -I/usr/local/inutil \
		  -I /usr/local/include/evhtp/ \

LIBS =  -L/usr/local/lib/  -levent -lrt -lssl -lcrypto \
		/usr/local/inutil/libinvutil.a /usr/local/lib/libevhtp.a  \
		-lboost_thread -lboost_system -pthread -levent -lssl -lcrypto -levent_openssl -lz -lcurl

objects = ./src/simple_httphandle.o \
		  ./src/http_util.o ./src/request_buffer.o 

serverexe = ./src/httpserver.o 
clientexe = ./src/client.o

all: server client

server: $(objects) $(serverexe)
	$(CC) $(CFLAGS) $(INCLUDE) -o ./bin/${MAIN} $^ $(LIBS) 

client: $(objects) $(clientexe)
	$(CC) $(CFLAGS) $(INCLUDE) -o ./bin/${CLIENT} $^ $(LIBS) 

$(objects): %.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(serverexe): %.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(clientexe): %.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -rf src/*.o bin/*
