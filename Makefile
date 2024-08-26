CC = g++
CFLAGS = -Wall -g -Werror

all: myserver myclient 

myserver: test_server.o socket.o state_machine.o tcb.o packet.o
	$(CC) $(CFLAGS) -o myserver test_server.o socket.o state_machine.o tcb.o packet.o

myclient: test_client.o socket.o state_machine.o tcb.o packet.o
	$(CC) $(CFLAGS) -o myclient test_client.o socket.o state_machine.o tcb.o packet.o

#mytcp: test_server.o test_client.o socket.o state_machine.o tcb.o packet.o
#	$(CC) $(CFLAGS) -o mytcp test_server.o test_client.o socket.o state_machine.o tcb.o packet.o

test_server.o: test_server.cpp
	$(CC) $(CFLAGS) -c test_server.cpp

test_client.o: test_client.cpp
	$(CC) $(CFLAGS) -c test_client.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

tcb.o: tcb.cpp
	$(CC) $(CFLAGS) -c tcb.cpp

socket.o: socket.cpp
	$(CC) $(CFLAGS) -c socket.cpp

state_machine.o: state_machine.cpp
	$(CC) $(CFLAGS) -c state_machine.cpp

packet.o: packet.cpp
	$(CC) $(CFLAGS) -c packet.cpp

clean:
	rm -f myserver myclient *.o
