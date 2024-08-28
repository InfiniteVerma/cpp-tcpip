#include "socket.h"
#include "iostream"
#include "ip.h"
#include "state_machine.h"
#include "tcp.h"
#include <arpa/inet.h>
#include <bitset>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

unsigned int getChecksumVal(void *ipHeader) {
    int length = sizeof(struct IPHeader);
    unsigned long checksum =
        0; // Use a larger type to handle potential overflow
    char *data =
        (char *)ipHeader; // Cast the IP header to a byte array for processing

    for (int i = 0; i + 1 < length;
         i += 2) // Iterate through the header two bytes (16 bits) at a time
    {
        unsigned int word; // word represents 16 bits (2 bytes)
        memcpy(&word, data + i,
               2);               // Copy 2 bytes from the header into the word
        checksum += ntohs(word); // Convert from network byte order to host byte
                                 // order and add to checksum

        if (checksum > 0xffff) // If overflow occurs (more than 16 bits), wrap
                               // around the checksum
        {
            checksum -= 0xffff; // Subtract to simulate 16-bit overflow
        }
    }

    if (length & 1) // If the length is odd, one byte remains to be processed
    {
        unsigned int word =
            0; // Initialize word to 0 to safely handle the last byte
        memcpy(&word, data + length - 1,
               1); // Copy the last byte into the lower byte of word
        checksum += ntohs(
            word); // Add the last byte (treated as a 16-bit word) to checksum
        if (checksum > 0xffff) {
            checksum -= 0xffff; // Handle overflow
        }
    }

    return htons(~checksum); // Return the one's complement of the checksum,
                             // converted back to network byte order
}

std::bitset<EPHEMERAL_PORT_SIZE> Socket::ephemeralPortStatusSet = {0};
int Socket::ephemeralPorts[EPHEMERAL_PORT_SIZE] = {
    1024, 1025, 1026, 1027, 1028}; // TODO how to dynamically do this?

Socket::Socket(std::string desc) {
    this->desc = desc;
    socketFd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

    if (socketFd == -1) {
        printf(
            "Name: %s: Oh dear, something went wrong with socket()! error: %d, "
            "%s\n",
            desc.c_str(), errno, strerror(errno));
        assert(0);
    }
    // socketFd = socket(AF_INET, SOCK_STREAM, 0);
}

void Socket::bind(int p) {
    tcb.localPortNum = p;

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("0.0.0.0");

    int ret = ::bind(socketFd, (struct sockaddr *)&serverAddress,
                     sizeof(serverAddress));

    if (ret != 0) {
        printf("Name: %s: Oh dear, something went wrong with bind()! error: "
               "%d, %s\n",
               desc.c_str(), errno, strerror(errno));
        assert(0);
    }

    // int flag = 1;
    // int result = setsockopt(socketFd, IPPROTO_TCP, TCP_NODELAY, (char
    // *)&flag, sizeof(int)); if (result < 0) {
    //     std::cerr << "Error disabling Nagle's algorithm\n";
    // }
}

// TODO handle case if same instance binds and connects to same port?
/*
 * If a socket calls 'connect', it implies it's a client and the param passed is
 * the port that client is connecting to
 *
 * 1. connect to the TCP (low level)
 * 2. Perform the 3-way handshake
 */
int Socket::connect(int serverPort) {
    // Set up the server address structure
    // sockaddr_in serverAddress;
    // serverAddress.sin_family = AF_INET;
    // serverAddress.sin_port = htons(serverPort);
    // serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // Replace with
    // actual server IP

    //// Allocate an ephemeral port number if needed (may not be necessary)
    // tcb.localPortNum = Socket::allocateEphemeralPortNum();

    //// Set up the socket for connection
    // int ret = ::connect(socketFd, (struct sockaddr*)&serverAddress,
    // sizeof(serverAddress)); if (ret != 0) {
    //     printf("Name: %s: Oh dear, something went wrong with connect()!
    //     error: %d, %s\n", desc.c_str(), errno, strerror(errno)); assert(0);
    // }

    // Perform the three-way handshake
    int ret = threeWayHandshakeClient();

    if (ret)
        cout << "Connected\n";
    return ret;
}

/*
 * Three way handshake as done by Client.
 *
 * 1. Sends SYN, updates state machine, starts timer, adds packet to
 * transmission queue
 * 2. Waits for SYN-ACK
 * 3. If received, delete packet from queue
 *    - Sends ACK, updates state machine
 *    - starts timer, adds pkt to transmission queue
 */
int Socket::threeWayHandshakeClient() {
    cout << __FUNCTION__ << " BEGIN\n";

    struct sockaddr_in destAddress;
    destAddress.sin_family = AF_INET;
    destAddress.sin_port = htons(0); // No port for raw sockets
    destAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    Packet pkt = getSYNPacket();

    const char *payload = pkt.makePacket();
    int size = pkt.getSize();

    cout << "Sending 1st SYN\n";
    sendto(socketFd, payload, size, 0, (struct sockaddr *)&destAddress,
           sizeof(destAddress));

    delete payload;

    sleep(2);
    cout << "1st SYN sent\n";

    // TODO start timer, retransmission queue

    return 1;
}

Packet Socket::getSYNPacket() {
    Packet packet(tcb.localPortNum, tcb.remotePortNum);
    return packet;
#if 0
#endif
}

/*
 * Server is now is a passive state, waiting for SYN pkts from a client.
 * Once it receives one, it responds to complete the 3-way handshake
 */
void Socket::listen() // TODO support backlog queue
{
    cout << "Starting to listen for SYN pkts\n";
    while (1) // TODO should it be a while loop?
    {
        struct sockaddr saddr;
        socklen_t socklen = sizeof(saddr);
        char *buffer = new char[65535];
        cout << "Starting recvfrom\n";
        ssize_t size = recvfrom(socketFd, buffer, 100000, 0, &saddr,
                                &socklen); // TODO make it blocking?

        cout << "recvfrom break\n";

        if (size == 0) {
            // peer has performed an orderly shutdown
        } else if (size == -1) {
            printf("Name: %s: Oh dear, something went wrong with recvfrom()! "
                   "error: "
                   "%d, %s\n",
                   desc.c_str(), errno, strerror(errno));
            assert(0);
        } else {
            cout << "Receive a packet of size: " << size << "\n";

            Utils::hexDump(buffer, size);

            cout << "After hex dump\n";
            char *msg = buffer + sizeof(IPHeader);
            cout << "Payload: " << msg << "\n";
        }
    }

    tcb.myState.updateState(LISTEN);
}

void Socket::send(const char *message, size_t len, int flags) {
    cout << "Client: sending: " << message << " - len: " << len << "\n";

    sockaddr_in saddr = {};
    // saddr.sin_family = AF_INET;
    // saddr.sin_port = htons(9000); // Destination port
    // saddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Destination IP

    //// Check for invalid IP address
    // if (saddr.sin_addr.s_addr == INADDR_NONE) {
    //     cout << "Invalid IP address\n";
    //     return;
    // }

    ssize_t bytesSent = sendto(socketFd, message, len, flags,
                               (sockaddr *)&saddr, sizeof(saddr));

    if (bytesSent == -1) {
        printf(
            "Name: %s: Oh dear, something went wrong with sendto()! error: %d, "
            "%s\n",
            desc.c_str(), errno, strerror(errno));
        assert(0);
    }
}

void Socket::close() { ::close(socketFd); }

/*
 * TODO this is complicated too. Server process isn't directly awake that 3-way
 * handshake has completed. When they make 'accept' call, the next completed
 * connection from the queue is returned
 */
int Socket::accept() { return ::accept(socketFd, nullptr, nullptr); }

int Socket::allocateEphemeralPortNum() {
    bool allocated = false;
    int i;

    for (i = 0; i < EPHEMERAL_PORT_SIZE; i++) {
        if (!ephemeralPortStatusSet[i]) {
            ephemeralPortStatusSet.set(i);
            allocated = true;
            break;
        }
    }

    if (!allocated) {
        printf("Oh dear, can't allocate an ephemeral port!");
        assert(0);
    }

    return ephemeralPorts[i];
}

void Socket::freeEphemeralPortNum(int portNum) {

    bool deallocated = false;

    for (int i = 0; i < EPHEMERAL_PORT_SIZE; i++) {
        if (ephemeralPorts[i] == portNum) {
            ephemeralPortStatusSet.reset(i);
            deallocated = true;
            break;
        }
    }

    if (!deallocated) {
        printf("Oh dear, can't deallocate ephemeral port: %d!", portNum);
        assert(0);
    }
}

void Socket::debugPrint() {
    cout << "==============\nSocket debugPrint\n";
    cout << "Port: " << tcb.localPortNum << "\n";
    cout << "TCB State: " << tcb.myState.getState() << "\n";
}
