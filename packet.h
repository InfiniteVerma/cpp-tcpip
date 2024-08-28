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

    bool isSYN();
    int getSeq();

  private:
    IPHeader ipHeader;
    TCPHeader tcpHeader;
    int size;
};

#endif
