#include "socket.h"
#include "iostream"
#include <cassert>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;

Socket::Socket(int p)
{
    tcb.localPortNum = p;

    socketFd = socket(AF_INET, SOCK_RAW, 0);
    // TODO init TCB
}

void
Socket::bind()
{
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int ret = ::bind(socketFd, (struct sockaddr*)&serverAddress,
         sizeof(serverAddress));

    if(!ret) assert(0);
}

void
Socket::listen()
{
    int ret = ::listen(socketFd, 5);

    if(!ret) assert(0);

    tcb.myState.updateState(LISTEN);
}

void
Socket::debugPrint()
{
    cout << "==============\nSocket debugPrint\n";
    cout << "Port: " << tcb.localPortNum << "\n";
    cout << "TCB State: " << tcb.myState.getState() << "\n";
}


