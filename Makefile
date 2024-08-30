CC = g++
CFLAGS = -Wall -g -Werror -I.

# List of source files
SRC = main.cpp packet.cpp socket.cpp tcb.cpp state_machine.cpp mytcp.cpp

#all: myserver myclient 
# List of object files
OBJ = $(SRC:.cpp=.o)

# Executable name
EXEC = mytcp

# Default target
all: $(EXEC) myserver myclient

# Rule to link the object files into the final executable
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

# Rule to compile each .cpp file into .o
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# for testing
myserver: test_server.o socket.o state_machine.o tcb.o packet.o mytcp.o
	$(CC) $(CFLAGS) -o myserver test_server.o socket.o state_machine.o tcb.o packet.o mytcp.o

myclient: test_client.o socket.o state_machine.o tcb.o packet.o mytcp.o
	$(CC) $(CFLAGS) -o myclient test_client.o socket.o state_machine.o tcb.o packet.o mytcp.o

# Clean target to remove generated files
clean:
	rm -f myserver myclient $(OBJ) $(EXEC) test_client.o test_server.o
