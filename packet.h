#ifndef __RFC793_PACKET__
#define __RFC793_PACKET__

#include "ip.h"
#include "tcp.h"

/*
 * Constructs a packet (payload + header)
 */
class Packet {
  public:
    Packet(int sourcePort, int destPort);
    Packet(char *rawPacket, int size);
    ~Packet();

    const char *makePacket();
    int getSize();

    int getSeq();

    static Packet getSYNPacket(int localPort, int removePort);

    // Checkers
    static bool isSYNPacket(Packet);

    // Actions
    static Packet getSynAckPacket(int localPort, int remotePort);

  private:
    IPHeader ipHeader;
    TCPHeader tcpHeader;
    int size;
};

#endif
