#include "socket.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <bitset>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <ctime>

#include "ip.h"
#include "mytcp.h"
#include "packet.h"
#include "state_machine.h"
#include "tcb.h"
#include "timer.h"
using namespace std;

unsigned int getChecksumVal(void *ipHeader) {
    int length = sizeof(struct IPHeader);
    unsigned long checksum = 0;     // Use a larger type to handle potential overflow
    char *data = (char *)ipHeader;  // Cast the IP header to a byte array for processing

    for (int i = 0; i + 1 < length; i += 2)  // Iterate through the header two bytes (16 bits) at a time
    {
        unsigned int word;  // word represents 16 bits (2 bytes)
        memcpy(&word, data + i,
               2);                // Copy 2 bytes from the header into the word
        checksum += ntohs(word);  // Convert from network byte order to host byte
                                  // order and add to checksum

        if (checksum > 0xffff)  // If overflow occurs (more than 16 bits), wrap
                                // around the checksum
        {
            checksum -= 0xffff;  // Subtract to simulate 16-bit overflow
        }
    }

    if (length & 1)  // If the length is odd, one byte remains to be processed
    {
        unsigned int word = 0;  // Initialize word to 0 to safely handle the last byte
        memcpy(&word, data + length - 1,
               1);                // Copy the last byte into the lower byte of word
        checksum += ntohs(word);  // Add the last byte (treated as a 16-bit word) to checksum
        if (checksum > 0xffff) {
            checksum -= 0xffff;  // Handle overflow
        }
    }

    return htons(~checksum);  // Return the one's complement of the checksum,
                              // converted back to network byte order
}

std::bitset<EPHEMERAL_PORT_SIZE> Socket::ephemeralPortStatusSet = {0};
int Socket::ephemeralPorts[EPHEMERAL_PORT_SIZE] = {1024, 1025, 1026, 1027, 1028};  // TODO how to dynamically do this?

Socket::Socket(std::string desc, const char *ip, int port) {
    this->desc = desc;
    socketFd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

    /*
     * Allocating memory and copying it before original pointer points to MyMsg instance that gets deleted after
     * reactToUserCalls ends
     */
    this->sourceIp = new char[strlen(ip) + 1];
    strcpy(sourceIp, ip);
    tcb.localPortNum = port;

    debugPrint();

    if (socketFd == -1) {
        printf(
            "Name: %s: Oh dear, something went wrong with socket()! error: %d, "
            "%s\n",
            desc.c_str(), errno, strerror(errno));
        assert(0);
    }
    // socketFd = socket(AF_INET, SOCK_STREAM, 0);

    resetSYNRetryCount();
}

Socket::~Socket() {
    delete[] sourceIp;
    delete[] destIp;
}

void Socket::setDestIp(const char *ip) {
    /*
     * Allocating memory and copying it before original pointer points to MyMsg instance that gets deleted after
     * reactToUserCalls ends
     */
    this->destIp = new char[strlen(ip) + 1];
    strcpy(destIp, ip);
}

void Socket::bind() {
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(sourceIp);

    LOG(__FUNCTION__);
    debugPrint();

    int ret = ::bind(socketFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    if (ret != 0) {
        printf(
            "Name: %s: Oh dear, something went wrong with bind()! error: "
            "%d, %s\n",
            desc.c_str(), errno, strerror(errno));
        assert(0);
    }

    // int flag = 1;
    // int result = setsockopt(socketFd, IPPROTO_TCP, TCP_NODELAY, (char
    // *)&flag, sizeof(int)); if (result < 0) {
    //     std::cerr , "Error disabling Nagle's algorithm\n";
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
int Socket::connect() {
    struct sockaddr_in clientAddress;
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_addr.s_addr = inet_addr(sourceIp);

    LOG(__FUNCTION__);
    debugPrint();

    if (::bind(socketFd, (struct sockaddr *)&clientAddress, sizeof(clientAddress)) < 0) {
        perror("Socket bind failed");
        ::close(socketFd);
        assert(0);
        return 1;
    }

    resetSYNRetryCount();
    int ret = startHandshake();  // TODO rename?

    return ret;
}

void Socket::retryHandshake() {
    LOG(__FUNCTION__, " BEGIN");
    incSYNRetryCount();
    startHandshake();
}

UINT32 Socket::getLastTransmittedSeqNumber()  // TODO a better way?
{
    return tcb.snd_nxt - 1;
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
int Socket::startHandshake() {
    // LOG(__FUNCTION__, " BEGIN\n");

    tcb.updateState(OPEN);  // To update FSM

    ACTION firstAction = tcb.updateState(NULL, 0);

    // LOG(__FUNCTION__, "Making packet with source IP: ", this->sourceIp, " dest IP: ", this->destIp);

    Packet pkt = firstAction(PktData(tcb.localPortNum, tcb.remotePortNum, tcb.iss, 0, sourceIp, destIp));

    tcb.snd_una = tcb.iss;
    tcb.snd_nxt = tcb.iss + 1;

    LOG("Sending 1st SYN seq:", tcb.iss);
    sendPacket(pkt);
    LOG("1st SYN sent");

    // debugPrint();

    return 0;
}

/*
 * Actually sends the packet out
 */
void Socket::sendPacket(Packet pkt) {
    const char *payload = pkt.makePacket();
    int size = pkt.getSize();

    struct sockaddr_in destAddress;
    destAddress.sin_family = AF_INET;
    destAddress.sin_port = htons(0);  // No port for raw sockets
    destAddress.sin_addr.s_addr = inet_addr(sourceIp);

    sendto(socketFd, payload, size, 0, (struct sockaddr *)&destAddress, sizeof(destAddress));

    delete[] payload;

    LOG(__FUNCTION__, "sending below packet");
    Utils::hexDump(payload, size);

    LOG(__FUNCTION__, "sendto done");
}

/*
 * Server is now is a passive state, waiting for SYN pkts from a client.
 * Once it receives one, it responds to complete the 3-way handshake
 */
void Socket::listen()  // TODO support backlog queue
{
    LOG("Starting to listen for SYN pkts\n");
    tcb.updateState(LISTEN);
    debugPrint();

#if 0
    char *buffer = new char[65535];
    int size = 0;
    // Receive SYN
    receivePacketNonBlocking(buffer, size);
    Utils::hexDump(buffer, size);

    LOG("After hex dump\n");
    LOG("Sending to update state\n");
    ACTION nextAction = tcb.updateState(buffer, size);

    LOG("Calling nextAction\n");
    Packet pkt = nextAction(PktData(tcb.localPortNum, tcb.remotePortNum, tcb.snd_nxt, 0, sourceIp, destIp));

    tcb.snd_nxt++;

    // Send SynAck
    sendPacket(pkt);

    debugPrint();

    // Receive ACK
    // now wait for 2 seconds for ACK message :)
    memset(buffer, 0, sizeof(char) * 65535);
    size = 0;
    int ret = receivePacketBlocking(buffer, size, 2);
    if (ret > 0) {
        Utils::hexDump(buffer, size);
        LOG("After hex dump\n");

        ACTION nextAction = tcb.updateState(buffer, size);

        LOG("Calling nextAction\n");
        if (!nextAction && tcb.getState() == ESTABLISHED) {
            LOG("Established!");
            return;
        } else {
            assert(0);
        }
    } else {
        LOG("No ACK received, stopping handshake!\n");
        assert(0);
    }
#endif
}

int Socket::receivePacketBlocking(char *buffer, int &size, int seconds) {
    // LOG(__FUNCTION__, " BEGIN ");
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(socketFd, &rfds);

    // Set timeout values based on the input parameter
    tv.tv_sec = seconds;
    tv.tv_usec = 0;

    // Use select to wait for data to be available
    int retval = select(socketFd + 1, &rfds, NULL, NULL, &tv);
    // LOG(__FUNCTION__, " select retuned ret val: ", retval);

    if (retval == -1) {
        perror("select()");
        return retval;
    } else if (retval) {
        // Data is available
        struct sockaddr_in saddr;
        socklen_t socklen = sizeof(saddr);

        // Ensure buffer is adequately sized and receive data
        size = recvfrom(socketFd, buffer, 1500000, 0, (struct sockaddr *)&saddr, &socklen);

        if (size == -1) {
            printf("Error: %d, %s\n", errno, strerror(errno));
            return size;
        } else if (size == 0) {
            // Peer has performed an orderly shutdown
            printf("Peer has performed an orderly shutdown\n");
        } else {
            // Successfully received data
            LOG(__FUNCTION__, " Received a packet of size: ", size);
        }
        return retval;
    } else {
        // No data within the timeout period
        // LOG(__FUNCTION__, " No data within ", seconds, " seconds");
        return 0;
    }
}

void Socket::receivePacketNonBlocking(char *buffer, int &size) {
    struct sockaddr saddr;
    socklen_t socklen = sizeof(saddr);
    size = recvfrom(socketFd, buffer, 100000, 0, &saddr,
                    &socklen);  // TODO make it blocking?

    LOG("recvfrom returned size: ", size);

    if (size == 0) {
        // peer has performed an orderly shutdown
    } else if (size == -1) {
        printf(
            "Name: %s: Oh dear, something went wrong with recvfrom()! "
            "error: "
            "%d, %s\n",
            desc.c_str(), errno, strerror(errno));
        assert(0);
    } else {
        LOG("Receive a packet of size: ", size);
    }
}

/*
 * Used to used the data packet. Input contains just the payload.
 *
 * It first adds the headers, constructors a Packet from it and then calls sendPacket
 */
void Socket::send(const char *message, size_t len, int flags) {
    LOG("Client: sending: ", message, " - len: ", len);

    LOG("Client: TODO fix the sequence logic");
    PktData pktData(tcb.localPortNum, tcb.remotePortNum, tcb.iss, 0, sourceIp, destIp);
    Packet packet = Packet::constructDataPacket(pktData, message, len);

    sendPacket(packet);
}

void Socket::close() {
    LOG(__FUNCTION__, " closing socket. TODO free any memory?");
    ::close(socketFd);
}

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
    LOG("==============\nSocket debugPrint");
    LOG("Port: ", tcb.localPortNum);
    LOG("Source IP: ", sourceIp);
    LOG("Dest IP: ", destIp);
    tcb.debugPrint();
}

ACTION Socket::updateState(const char *buf, int size) {
    LOG(__FUNCTION__, " BEGIN");
    return tcb.updateState(buf, size);
}

void Socket::executeNextAction(ACTION action, char *buffer, int size) {
    LOG(__FUNCTION__, " BEGIN");
    if (!action) {
        LOG(__FUNCTION__, " action is NULL, current statue: ", tcb.getState(), " returning!");
    } else {
        Packet p = Packet(buffer, size);
        Packet nextPkt;
        Timer *timerInstance = Timer::getInstance();

        tcb.snd_nxt = tcb.iss + 1;
        tcb.snd_una = tcb.iss;

        switch (tcb.getState()) {
            case SYN_RECEIVED: {
                LOG(__FUNCTION__, " I switched from LISTEN TO SYN_RECEIVED");
                tcb.rcv_nxt = p.getSeq() + 1;
                tcb.irs = p.getSeq();  // TODO delete irs func
                nextPkt = action(PktData(tcb.localPortNum, tcb.remotePortNum, tcb.rcv_nxt, tcb.iss, sourceIp, destIp));
                timerInstance->delTimers(p.getAck());

                // ScheduledTask *task = new ScheduledTask(3.0, []() {
                //     LOG("TIMEOUT HIT for HANDSHAKE!!!!");

                //    MyTcp::setRetVal(1);
                //});

                // timerInstance->addTimer(nextPkt.getSeq(), task);

                sendPacket(nextPkt);
                break;
            }
            case ESTABLISHED: {
                LOG(__FUNCTION__, " I switched from SYN_SENT TO ESTABLISHED");
                tcb.rcv_nxt = p.getSeq() + 1;
                tcb.irs = p.getSeq();
                tcb.snd_una = p.getAck();
                timerInstance->delTimer(p.getAck());
                nextPkt =
                    action(PktData(tcb.localPortNum, tcb.remotePortNum, tcb.rcv_nxt, tcb.snd_nxt, sourceIp, destIp));
                MyTcp::setRetVal(0);

                sendPacket(nextPkt);
                break;
            }
            case CLOSED: {
                LOG(__FUNCTION__, " I got a packet while socket is closed, dropping the packet");
                break;
            }
            default:
                // TODO handle
                LOG(__FUNCTION__, " case: " + enumToName(tcb.getState()) + " not handled");
                assert(0);
                break;
        }
    }
}

ConnectionState Socket::getCurrentState() { return tcb.getState(); }
