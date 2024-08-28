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
    ~Packet();

    const char *makePacket();
    int getSize();

  private:
    TCPHeader tcpHeader;
    IPHeader ipHeader;
    int size;
};

#endif
