#include <cerrno>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include "socket.h"

#define PORT 8000

void server()
{
    std::cout << "Hello from server thread\n";
    Socket socket("Server");

    socket.bind(8000);
    socket.listen();

    socket.debugPrint();

    while(1)
    {
        int clientSocket = socket.accept();
        std::cout << "SERVER: clientSocket: " << clientSocket << "\n";

        if(clientSocket == -1)
        {
            printf("Oh dear, something went wrong with accept()! %s\n", strerror(errno));
            break;
        }

        char buffer[1024] = { 0 };
        ::recv(clientSocket, buffer, sizeof(buffer), 0);
        std::cout << "SERVER: Message from client: <" << buffer << ">" << std::endl;

        if(strcmp(buffer, "CLOSE")==0)
        {
            std::cout << "Closing server\n";
            break;
        }
        else
        {
            std::cout << "SERVER: buffer doesn't match close: " << buffer << "\n";
        }
    }
    socket.close();
}

void client()
{
    std::cout << "Hello from client thread\n";
    Socket socket("Client");

    socket.connect(8000);

    const char* message = "CLOSE";
    socket.send(message, strlen(message), 0);
    socket.close();
}

int main()
{
    std::thread serverThread(server);
    std::thread clientThread(client);
    clientThread.join();
    serverThread.join();
}
