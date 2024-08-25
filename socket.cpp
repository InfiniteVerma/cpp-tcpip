#include "socket.h"
#include "iostream"
#include "state_machine.h"
#include <cassert>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
using namespace std;

Socket::Socket(std::string desc)
{
    //socketFd = socket(AF_INET, SOCK_RAW, 0);
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    // TODO init TCB
    
    this->desc = desc;
}

void
Socket::bind(int p)
{
    tcb.localPortNum = p;

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(tcb.localPortNum);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int ret = ::bind(socketFd, (struct sockaddr*)&serverAddress,
         sizeof(serverAddress));

    if(ret != 0)
    {
        printf("Name: %s: Oh dear, something went wrong with bind()! error: %d, %s\n", desc.c_str(), errno, strerror(errno));
        assert(0);
    }

    int flag = 1;
    int result = setsockopt(socketFd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
    if (result < 0) {
        std::cerr << "Error disabling Nagle's algorithm\n";
    }
}

// TODO handle case if same instance binds and connects to same port?
void
Socket::connect(int p)
{
    tcb.localPortNum = p;

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(tcb.localPortNum);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // sending connection request
    ::connect(socketFd, (struct sockaddr*)&serverAddress,
            sizeof(serverAddress));
}

void
Socket::listen()
{
    int ret = ::listen(socketFd, 5);

    if(ret != 0)
    {
        printf("Name: %s: Oh dear, something went wrong with bind()! error: %d, %s\n", desc.c_str(), errno, strerror(errno));
        assert(0);
    }

    tcb.myState.updateState(LISTEN);
}

void
Socket::send(const char* message, size_t len, int flags) // TODO flags?
{
    cout << "Client: sending: " << message << "\n";
    ::send(socketFd, message, len, 0);
}

void
Socket::close()
{
    ::close(socketFd);
}

int
Socket::accept()
{
    return ::accept(socketFd, nullptr, nullptr);
}

void
Socket::debugPrint()
{
    cout << "==============\nSocket debugPrint\n";
    cout << "Port: " << tcb.localPortNum << "\n";
    cout << "TCB State: " << tcb.myState.getState() << "\n";
}
