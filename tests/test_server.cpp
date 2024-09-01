#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cstring>

#include "mytcp.h"
#include "user_socket.h"

using namespace std;

#define PORT 9000

/*
 * Server: 192.168.1.1
 * Client: 192.168.1.2
 */

void server() {
    LOG("Hello from server thread");

    string name = "Server";
    string srcIp = "192.168.1.1";
    string destIp = "192.168.1.2";

    UINT8 fd = UserSocket::create(name, srcIp, destIp, PORT);
    LOG("Got fd: ", fd);

    int ret = UserSocket::bind(fd);
    LOG("bind ret: ", ret);

    assert("ret val is not 0" && ret == 0);

    ret = UserSocket::listen(fd, 1);
    LOG("listen ret: ", ret);

    while (1) {
        int clientSocketFD = UserSocket::accept(fd);  // returns when there is a packet available
        LOG(__FUNCTION__, "Accept request from a client! Will now block until it sends a packet");

        char* buffer = new char[1024];
        int size = UserSocket::receive(clientSocketFD, buffer, sizeof(buffer), 0);

        if (size > 0) {
        } else {
            LOG(__FUNCTION__, " error size is not valid: ", size);
            break;
        }
    }

    ret = UserSocket::close(fd);
    LOG("close ret: ", ret);
}

int main() {
    MyTcp::createMyTCP("Server");
    server();
    LOG("Stopping tcp thread");
    UserSocket::stopTCP();
    LOG("Stopping client thread and exiting");
    return 0;
}
