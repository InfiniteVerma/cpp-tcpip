CC = g++
CFLAGS = -Wall -g

all: mytcp 

mytcp: main.o socket.o state_machine.o tcb.o
	$(CC) $(CFLAGS) -o mytcp main.o socket.o state_machine.o tcb.o

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

tcb.o: tcb.cpp
	$(CC) $(CFLAGS) -c tcb.cpp

socket.o: socket.cpp
	$(CC) $(CFLAGS) -c socket.cpp

state_machine.o: state_machine.cpp
	$(CC) $(CFLAGS) -c state_machine.cpp

clean:
	rm -f mytcb *.o
