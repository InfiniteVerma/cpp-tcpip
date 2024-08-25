CC = g++
CFLAGS = -Wall -g

all: mytcp

mytcp: main.o tcp.o
	$(CC) $(CFLAGS) -o mytcp main.o tcp.o

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

tcp.o: tcp.cpp
	$(CC) $(CFLAGS) -c tcp.cpp

clean:
	rm -f mytcp main.o tcp.o
