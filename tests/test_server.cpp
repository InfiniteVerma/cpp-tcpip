#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cstring>
#include <iostream>

#include "mytcp.h"
#include "user_socket.h"

using namespace std;

#define PORT 9000

/*
 * Server: 192.168.1.1
 * Client: 192.168.1.2
 */

void server() {
    std::cout << "Hello from server thread\n";
#if 0
    Socket socket("Server", "192.168.1.1", PORT);

    socket.setDestIp("192.168.1.2");
    socket.bind();
    socket.listen();

    socket.debugPrint();

#endif
    string name = "Server";
    string srcIp = "192.168.1.1";
    string destIp = "192.168.1.2";

    int fd = UserSocket::create(name, srcIp, destIp, PORT);

    cout << "Got fd: " << fd << "\n";

    int ret = UserSocket::bind(fd);

    cout << "bind ret: " << ret << "\n";

    assert("ret val is not 0" && ret == 0);

    ret = UserSocket::listen(fd);

    cout << "TODO handshake done!\n";
    sleep(200);
    // while (1) {
    //     int clientSocket = socket.accept();
    //     std::cout << "SERVER: clientSocket: " << clientSocket << "\n";

    //    if (clientSocket == -1) {
    //        printf("Oh dear, something went wrong with accept()! %s\n", strerror(errno));
    //        break;
    //    }

    //    char buffer[1024] = {0};
    //    ::recv(clientSocket, buffer, sizeof(buffer), 0);
    //    std::cout << "SERVER: Message from client: <" << buffer << ">" << std::endl;

    //    if (strcmp(buffer, "CLOSE") == 0) {
    //        std::cout << "Closing server\n";
    //        break;
    //    } else {
    //        std::cout << "SERVER: buffer doesn't match close: " << buffer << "\n";
    //    }
    //}
    // socket.close();
}

int main() {
    MyTcp::createMyTCP();
    server();
    return 0;
}
