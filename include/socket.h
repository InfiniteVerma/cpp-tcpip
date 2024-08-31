#ifndef __RFC793_SOCKET__
#define __RFC793_SOCKET__

#include <bitset>

#include "packet.h"
#include "tcb.h"
#include "tcp.h"

#define EPHEMERAL_PORT_SIZE 5

class Socket {
   public:
    Socket(std::string s, const char *ip, int port);

    void setDestIp(const char *ip);
    void debugPrint();

    /* Public APIs */
    void bind();
    int connect();
    void listen();
    void send(const char *message, size_t len, int flags);
    void close();
    int accept();

    static int allocateEphemeralPortNum();
    static void freeEphemeralPortNum(int portNum);

   private:
    int threeWayHandshakeClient();
    // Packet getSYNPacket();

    int socketFd;
    TCB tcb;
    std::string desc;
    const char *sourceIp;
    const char *destIp;

    void receivePacketNonBlocking(char *, int &);
    int receivePacketBlocking(char *, int &, int seconds);

    void sendPacket(Packet pkt);

    static int ephemeralPorts[EPHEMERAL_PORT_SIZE];  // TODO 10?
    static std::bitset<EPHEMERAL_PORT_SIZE> ephemeralPortStatusSet;

    queue<Packet> retransmissionQueue;
};

#endif