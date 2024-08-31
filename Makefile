# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -g -Werror -Iinclude

# List of source files
SRC = src/main.cpp src/packet.cpp src/socket.cpp src/tcb.cpp src/state_machine.cpp src/mytcp.cpp src/user_socket.cpp src/messages.cpp

# List of object files
OBJ = $(SRC:.cpp=.o)

# Executable name for the main application
EXEC = mytcp

# Include the test Makefile
include tests/Makefile

# Default target
all: $(EXEC) $(SERVER_EXEC) $(CLIENT_EXEC)

# Rule to link the object files into the final executable
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

# Rule to compile each .cpp file into .o
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target to remove generated files
clean:
	rm -f $(EXEC) $(OBJ) $(SERVER_EXEC) $(CLIENT_EXEC) tests/*.o

