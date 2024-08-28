#ifndef __RFC793_SOCKET__
#define __RFC793_SOCKET__

#include "packet.h"
#include "tcb.h"
#include "tcp.h"
#include <bitset>

#define EPHEMERAL_PORT_SIZE 5

class Socket {
  public:
    Socket(std::string s);

    void debugPrint();

    /* Public APIs */
    void bind(int p);
    int connect(int p);
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

    static int ephemeralPorts[EPHEMERAL_PORT_SIZE]; // TODO 10?
    static std::bitset<EPHEMERAL_PORT_SIZE> ephemeralPortStatusSet;

    queue<Packet> retransmissionQueue;
};

#endif
