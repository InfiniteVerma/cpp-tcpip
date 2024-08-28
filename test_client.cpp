#include "socket.h"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 9000

void client() {
  std::cout << "Hello from client thread\n";
  Socket socket("Client");

  int ret = socket.connect(PORT);

  socket.debugPrint();

  if (ret != 0) {
    // TODO formalize error coding
    std::cout << "3 way handshake failed\n";
    socket.close();
    return;
  }

  const char *message = "CLOSE";
  socket.send(message, strlen(message), 0);
  socket.close();
}

int main() {
  client();
  return 0;
}
