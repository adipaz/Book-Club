CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system

all: Client
	g++ -pthread -o bin/StompBookClubClient bin/connectionHandler.o bin/EncoderDecoder.o bin/Frame.o bin/echoClient.o bin/User.o bin/IOThread.o bin/ServerThread.o  $(LDFLAGS) 

Client: bin/connectionHandler.o bin/EncoderDecoder.o bin/Frame.o bin/echoClient.o bin/User.o bin/IOThread.o bin/ServerThread.o 
	
bin/connectionHandler.o: src/connectionHandler.cpp
	g++ -pthread $(CFLAGS) -o bin/connectionHandler.o src/connectionHandler.cpp

bin/EncoderDecoder.o: src/EncoderDecoder.cpp
	g++ -pthread $(CFLAGS) -o bin/EncoderDecoder.o src/EncoderDecoder.cpp

bin/Frame.o: src/Frame.cpp
	g++ -pthread $(CFLAGS) -o bin/Frame.o src/Frame.cpp

bin/echoClient.o: src/echoClient.cpp
	g++ -pthread $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp

bin/User.o: src/User.cpp
	g++ -pthread $(CFLAGS) -o bin/User.o src/User.cpp

bin/IOThread.o: src/IOThread.cpp
	g++ -pthread $(CFLAGS) -o bin/IOThread.o src/IOThread.cpp

bin/ServerThread.o: src/ServerThread.cpp
	g++ -pthread $(CFLAGS) -o bin/ServerThread.o src/ServerThread.cpp
	
.PHONY: clean
clean:
	rm -f bin/*
