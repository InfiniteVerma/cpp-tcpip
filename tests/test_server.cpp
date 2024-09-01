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
    LOG("Hello from server thread\n");

    string name = "Server";
    string srcIp = "192.168.1.1";
    string destIp = "192.168.1.2";

    UINT8 fd = UserSocket::create(name, srcIp, destIp, PORT);

    LOG("Got fd: ", fd, "\n");

    int ret = UserSocket::bind(fd);

    LOG("bind ret: ", ret, "\n");

    assert("ret val is not 0" && ret == 0);

    ret = UserSocket::listen(fd);

    LOG("listen ret: ", ret);

    ret = UserSocket::close(fd);

    LOG("close ret: ", ret);

    LOG("TODO listen handshake done!");
    // while (1) {
    //     int clientSocket = socket.accept();
    //     gLOG("SERVER: clientSocket: " , clientSocket , "\n");

    //    if (clientSocket == -1) {
    //        printf("Oh dear, something went wrong with accept()! %s\n", strerror(errno));
    //        break;
    //    }

    //    char buffer[1024] = {0};
    //    ::recv(clientSocket, buffer, sizeof(buffer), 0);
    //    gLOG("SERVER: Message from client: <" , buffer , ">" , std::endl);

    //    if (strcmp(buffer, "CLOSE") == 0) {
    //        gLOG("Closing server\n");
    //        break;
    //    } else {
    //        gLOG("SERVER: buffer doesn't match close: " , buffer , "\n");
    //    }
    //}
    // socket.close();
}

int main() {
    MyTcp::createMyTCP("Server");
    server();
    LOG("Stopping kernel thread");
    UserSocket::stopTCP();
    LOG("Stopping client thread and exiting");
    return 0;
}
