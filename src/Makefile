INCLUDE = -I../third_party/pistache/include/ -I../third_party -I.
LINK = -L../third_party/pistache/build-gcc/src/
LIB = -lshpa3 -lpthread -lpistache -lboost_system
FLAG = --std=c++11 -g


all: main

main: main.o phone_server.o synway.o
	g++ $(FLAG) -o main main.o phone_server.o synway.o $(INCLUDE) $(LINK) $(LIB) 

main.o: main.cc synway.h phone_server.h
	g++ $(FLAG) -c main.cc $(INCLUDE)

phone_server.o: phone_server.h phone_server.cc
	g++ $(FLAG) -c phone_server.cc $(INCLUDE)

synway.o: synway.cc synway.h
	g++ $(FLAG) -c synway.cc $(INCLUDE)

clean:
	rm *.o
