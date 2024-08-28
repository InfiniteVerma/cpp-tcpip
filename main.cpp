#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <thread>

#include "socket.h"

#define PORT 9000

void server() {
    std::cout << "Hello from server thread\n";
    Socket socket("Server", "192.168.1.1", PORT);

    socket.bind();
    socket.listen();

    socket.debugPrint();

    while (1) {
        int clientSocket = socket.accept();
        std::cout << "SERVER: clientSocket: " << clientSocket << "\n";

        if (clientSocket == -1) {
            printf("Oh dear, something went wrong with accept()! %s\n", strerror(errno));
            break;
        }

        char buffer[1024] = {0};
        ::recv(clientSocket, buffer, sizeof(buffer), 0);
        std::cout << "SERVER: Message from client: <" << buffer << ">" << std::endl;

        if (strcmp(buffer, "CLOSE") == 0) {
            std::cout << "Closing server\n";
            break;
        } else {
            std::cout << "SERVER: buffer doesn't match close: " << buffer << "\n";
        }
    }
    socket.close();
}

void client() {
    std::cout << "Hello from client thread\n";
    Socket socket("Client", "192.168.1.2", PORT);

    int ret = socket.connect();

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
    std::thread serverThread(server);
    std::thread clientThread(client);
    clientThread.join();
    serverThread.join();
}
