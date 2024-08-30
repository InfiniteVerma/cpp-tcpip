#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>

#include "socket.h"

#include "mytcp.h"

#define PORT 9000

/*
 * Server: 192.168.1.1
 * Client: 192.168.1.2
 */

void server() {
    std::cout << "Hello from server thread\n";
    Socket socket("Server", "192.168.1.1", PORT);

    socket.setDestIp("192.168.1.2");
    socket.bind();
    socket.listen();

    socket.debugPrint();

    cout << "TODO handshake done!\n";
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
    socket.close();
}

int main() {
    MyTcp::createMyTCP();
    server();
    return 0;
}
