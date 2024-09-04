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
    LOG(__FUNCTION__, "Hello from client thread");

    string name = "Client";
    string srcIp = "192.168.1.2";
    string destIp = "192.168.1.1";

    int fd = UserSocket::create(name, srcIp, destIp, PORT);

    LOG(__FUNCTION__, "create fd: ", fd);

    int ret = UserSocket::connect(fd);

    LOG(__FUNCTION__, "connect ret: ", ret);

    const char* message = "Hello, world";

    ret = UserSocket::send(fd, message, strlen(message), 0);
    LOG(__FUNCTION__, "send ret: ", ret);

    ret = UserSocket::close(fd);

    LOG(__FUNCTION__, "close ret: ", ret);
}

int main() {
    MyTcp::createMyTCP("Client");
    client();
    LOG(__FUNCTION__, "Stopping tcp thread");
    UserSocket::stopTCP();
    LOG(__FUNCTION__, "Stopping client thread and exiting");
    return 0;
}
