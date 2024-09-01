#ifndef __RFC793_SOCKET__
#define __RFC793_SOCKET__

#include <bitset>

#include "packet.h"
#include "state_machine.h"
#include "tcb.h"
#include "tcp.h"

#define EPHEMERAL_PORT_SIZE 5

class Socket {
   public:
    Socket(std::string s, const char *ip, int port);
    ~Socket();

    void setDestIp(const char *ip);
    void debugPrint();

    /* Public APIs */
    void bind();
    int connect();
    void listen();
    void send(const char *message, size_t len, int flags);
    void close();
    int accept();
    bool shouldListen();
    ACTION updateState(char *buf, int size);
    void executeNextAction(ACTION action);
    ConnectionState getCurrentState();

    static int allocateEphemeralPortNum();
    static void freeEphemeralPortNum(int portNum);
    void receivePacketNonBlocking(char *, int &);
    int receivePacketBlocking(char *, int &, int seconds);

   private:
    int threeWayHandshakeClient();
    // Packet getSYNPacket();

    int socketFd;
    TCB tcb;
    std::string desc;
    char *sourceIp;
    char *destIp;

    void sendPacket(Packet pkt);

    static int ephemeralPorts[EPHEMERAL_PORT_SIZE];  // TODO 10?
    static std::bitset<EPHEMERAL_PORT_SIZE> ephemeralPortStatusSet;

    queue<Packet> retransmissionQueue;
};

#endif
