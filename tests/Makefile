# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -g -Werror -I./include

# List of test source files
TEST_SRC_CLIENT = tests/test_client.cpp
TEST_SRC_SERVER = tests/test_server.cpp

# List of test object files
TEST_OBJ_CLIENT = $(TEST_SRC_CLIENT:.cpp=.o)
TEST_OBJ_SERVER = $(TEST_SRC_SERVER:.cpp=.o)

# Test executables
CLIENT_EXEC = myclient 
SERVER_EXEC = myserver

# Default target
all: $(CLIENT_EXEC) $(SERVER_EXEC)

# Rule to compile test_client object file
$(TEST_OBJ_CLIENT): $(TEST_SRC_CLIENT)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to compile test_server object file
$(TEST_OBJ_SERVER): $(TEST_SRC_SERVER)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to link test_client executable
$(CLIENT_EXEC): $(TEST_OBJ_CLIENT) ./src/packet.o ./src/socket.o ./src/state_machine.o ./src/tcb.o ./src/mytcp.o ./src/user_socket.o ./src/messages.o src/timer.o src/common.o
	$(CC) $(CFLAGS) -o $(CLIENT_EXEC) $(TEST_OBJ_CLIENT) ./src/packet.o ./src/socket.o ./src/state_machine.o ./src/tcb.o ./src/mytcp.o ./src/user_socket.o ./src/messages.o src/timer.o src/common.o

# Rule to link test_server executable
$(SERVER_EXEC): $(TEST_OBJ_SERVER) ./src/packet.o ./src/socket.o ./src/state_machine.o ./src/tcb.o ./src/mytcp.o ./src/user_socket.o ./src/messages.o src/timer.o src/common.o
	$(CC) $(CFLAGS) -o $(SERVER_EXEC) $(TEST_OBJ_SERVER) ./src/packet.o ./src/socket.o ./src/state_machine.o ./src/tcb.o ./src/mytcp.o ./src/user_socket.o ./src/messages.o src/timer.o src/common.o

# Clean target to remove generated files
clean:
	rm -f $(CLIENT_EXEC) $(SERVER_EXEC) $(TEST_OBJ_CLIENT) $(TEST_OBJ_SERVER)

