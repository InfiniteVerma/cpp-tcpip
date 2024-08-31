#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>

#include "mytcp.h"
// #include "socket.h"
#include "user_socket.h"

#define PORT 9000

/*
 * Server: 192.168.1.1
 * Client: 192.168.1.2
 */

void client() {
    std::cout << "Hello from client thread\n";
#if 0
    Socket socket("Client", "192.168.1.2", PORT);
    socket.setDestIp("192.168.1.1");

    int ret = socket.connect();

    socket.debugPrint();

    if (ret != 0) {
        // TODO formalize error coding
        std::cout << "3 way handshake failed\n";
        socket.close();
        return;
    }

#endif
    string name = "Client";
    string srcIp = "192.168.1.2";
    string destIp = "192.168.1.1";

    int fd = UserSocket::create(name, srcIp, destIp, PORT);

    cout << "fd: " << fd << "\n";

    int ret = UserSocket::connect(fd);

    cout << "connect ret: " << ret << "\n";

    cout << "Client TODO\n";

    // const char *message = "CLOSE";
    // socket.send(message, strlen(message), 0);
    // socket.close();
}

int main() {
    MyTcp::createMyTCP();
    client();
    cout << "Stopping the threads\n";
    MyTcp::stopMyTCP();
    return 0;
}
