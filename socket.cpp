#include "socket.h"
#include "iostream"
#include "packet.h"
#include "state_machine.h"
#include <arpa/inet.h>
#include <bitset>
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

std::bitset<EPHEMERAL_PORT_SIZE> Socket::ephemeralPortStatusSet = {0};
int Socket::ephemeralPorts[EPHEMERAL_PORT_SIZE] = {1024, 1025, 1026, 1027, 1028}; // TODO how to dynamically do this?

Socket::Socket(std::string desc)
{
   this->desc = desc;
   socketFd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

   if(socketFd == -1)
   {
       printf("Name: %s: Oh dear, something went wrong with socket()! error: %d, %s\n", desc.c_str(), errno, strerror(errno));
       assert(0);
   }
   // socketFd = socket(AF_INET, SOCK_STREAM, 0);
    
}

void
Socket::bind(int p)
{
    tcb.localPortNum = p;

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("0.0.0.0");

    int ret = ::bind(socketFd, (struct sockaddr*)&serverAddress,
         sizeof(serverAddress));

    if(ret != 0)
    {
        printf("Name: %s: Oh dear, something went wrong with bind()! error: %d, %s\n", desc.c_str(), errno, strerror(errno));
        assert(0);
    }

    //int flag = 1;
    //int result = setsockopt(socketFd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
    //if (result < 0) {
    //    std::cerr << "Error disabling Nagle's algorithm\n";
    //}
}

// TODO handle case if same instance binds and connects to same port?
/*
 * If a socket calls 'connect', it implies it's a client and the param passed is the port that client is connecting to
 *
 * 1. connect to the TCP (low level)
 * 2. Perform the 3-way handshake
 */
int Socket::connect(int serverPort) {
    // Set up the server address structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // Replace with actual server IP

    // Allocate an ephemeral port number if needed (may not be necessary)
    tcb.localPortNum = Socket::allocateEphemeralPortNum(); 

    // Set up the socket for connection
    int ret = ::connect(socketFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (ret != 0) {
        printf("Name: %s: Oh dear, something went wrong with connect()! error: %d, %s\n", desc.c_str(), errno, strerror(errno));
        assert(0);
    }

    cout << "Connected\n";

    // Perform the three-way handshake
    ret = threeWayHandshakeClient();
    return ret;
}


/*
 * Three way handshake as done by Client.
 *
 * 1. Sends SYN, updates state machine, starts timer, adds packet to transmission queue
 * 2. Waits for SYN-ACK
 * 3. If received, delete packet from queue
 *    - Sends ACK, updates state machine
 *    - starts timer, adds pkt to transmission queue
 */
int
Socket::threeWayHandshakeClient()
{
    Packet* synPacket = new Packet(tcb.localPortNum, tcb.remotePortNum);
    //const char* message = synPacket->makePacket();

    const char* message = "Hello, world";

    send(message, strlen(message), 0);
    cout << "Sent SYN\n";
    delete synPacket;
    return 1;
}

/*
 * Server is now is a passive state, waiting for SYN pkts from a client. 
 * Once it receives one, it responds to complete the 3-way handshake
 */
void
Socket::listen() // TODO support backlog queue
{
    //int ret = ::listen(socketFd, 1);
    cout << "Starting to listen for SYN pkts\n";
    while(1) // TODO should it be a while loop?
    {
        struct sockaddr saddr;
        socklen_t socklen = sizeof(saddr);
        char* buffer = new char[1500];
        ssize_t size = recvfrom(socketFd, &buffer, sizeof(buffer), 0, &saddr, &socklen); // TODO make it blocking?

        if(size == 0)
        {
            // peer has performed an orderly shutdown
        }
        else if(size == -1)
        {
            printf("Name: %s: Oh dear, something went wrong with recvfrom()! error: %d, %s\n", desc.c_str(), errno, strerror(errno));
            assert(0);
        }
        else
        {
            std::cout << "Receive a packet of size: " << size << "\n";
        }
    }

    tcb.myState.updateState(LISTEN);
}

void Socket::send(const char* message, size_t len, int flags) {
    cout << "Client: sending: " << message << " - len: " << len << "\n";

    sockaddr_in saddr = {};
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9000); // Destination port
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Destination IP

    // Check for invalid IP address
    if (saddr.sin_addr.s_addr == INADDR_NONE) {
        cout << "Invalid IP address\n";
        return;
    }

    ssize_t bytesSent = ::sendto(socketFd, message, len, flags, (sockaddr*)&saddr, sizeof(saddr));

    if (bytesSent == -1) {
        printf("Name: %s: Oh dear, something went wrong with sendto()! error: %d, %s\n", desc.c_str(), errno, strerror(errno));
        assert(0);
    }
}

void
Socket::close()
{
    ::close(socketFd);
}

/*
 * TODO this is complicated too. Server process isn't directly awake that 3-way handshake has completed.
 * When they make 'accept' call, the next completed connection from the queue is returned
 */
int
Socket::accept()
{
    return ::accept(socketFd, nullptr, nullptr);
}

int
Socket::allocateEphemeralPortNum()
{
    bool allocated = false;
    int i;

    for(i=0;i<EPHEMERAL_PORT_SIZE;i++)
    {
        if(!ephemeralPortStatusSet[i])
        {
            ephemeralPortStatusSet.set(i);
            allocated = true;
            break;
        }
    }

    if(!allocated)
    {
        printf("Oh dear, can't allocate an ephemeral port!");
        assert(0);
    }

    return ephemeralPorts[i];
}

void
Socket::freeEphemeralPortNum(int portNum)
{
    bool deallocated = false;

    for(int i=0;i<EPHEMERAL_PORT_SIZE;i++)
    {
        if(ephemeralPorts[i] == portNum)
        {
            ephemeralPortStatusSet.reset(i);
            deallocated = true;
            break;
        }
    }

    if(!deallocated)
    {
        printf("Oh dear, can't deallocate ephemeral port: %d!", portNum);
        assert(0);
    }
}

void
Socket::debugPrint()
{
    cout << "==============\nSocket debugPrint\n";
    cout << "Port: " << tcb.localPortNum << "\n";
    cout << "TCB State: " << tcb.myState.getState() << "\n";
}
