#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>

#include "mytcp.h"
#include "user_socket.h"

#define PORT 9000

/*
 * Server: 192.168.1.1
 * Client: 192.168.1.2
 */

void client() {
    LOG("Hello from client thread");

    string name = "Client";
    string srcIp = "192.168.1.2";
    string destIp = "192.168.1.1";

    int fd = UserSocket::create(name, srcIp, destIp, PORT);

    LOG("fd: ", fd);

    int ret = UserSocket::connect(fd);

    LOG("connect ret: ", ret);

    const char* message = "Hello, world";

    ret = UserSocket::send(fd, message, strlen(message), 0);
    LOG("send ret: ", ret);

    ret = UserSocket::close(fd);

    LOG("close ret: ", ret);
}

int main() {
    MyTcp::createMyTCP("Client");
    client();
    LOG("Stopping kernel thread");
    UserSocket::stopTCP();
    LOG("Stopping client thread and exiting");
    return 0;
}
