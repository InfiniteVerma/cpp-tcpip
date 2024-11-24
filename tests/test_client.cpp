#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>

#include "mytcp.h"
#include "user_socket.h"

#define PORT 9000

/*
 * Server: 192.168.1.2
 * Client: 192.168.1.3
 */

void runClient() {
    LOG(__FUNCTION__, "Hello from client thread");

    string name = "Client";
    string srcIp = "192.168.1.3";
    string destIp = "192.168.1.2";

    int fd = UserSocket::create(name, srcIp, destIp, PORT);

    LOG(__FUNCTION__, "create fd: ", fd);

    int ret = UserSocket::connect(fd);

    LOG(__FUNCTION__, "connect ret: ", ret);

    const char* message = "Hello, world";
    ret = UserSocket::send(fd, message, strlen(message), 0);

    message = "Hello again!";
    ret = UserSocket::send(fd, message, strlen(message), 0);

    message = "CLOSE";
    ret = UserSocket::send(fd, message, strlen(message), 0);

    ret = UserSocket::close(fd);

    LOG(__FUNCTION__, "close ret: ", ret);
}

int main() {
    MyTcp::createMyTCP("Client");
    runClient();
    LOG(__FUNCTION__, "Stopping tcp thread");
    UserSocket::stopTCP();
    LOG(__FUNCTION__, "Stopping client thread and exiting");
    return 0;
}
