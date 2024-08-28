#ifndef __RFC793_PACKET__
#define __RFC793_PACKET__

#include "ip.h"
#include "tcp.h"

struct PktData {
    int localPortNum;
    int remotePortNum;
    int nextExpectedSeqNumber; // 1 + seq get from older pkt
    const char *sourceIp;
    const char *destIp;

    PktData(int localP, int remoteP, int ackNo, const char *sourceIp,
            const char *destIp) {
        localPortNum = localP;
        remotePortNum = remoteP;
        nextExpectedSeqNumber = ackNo;
        this->sourceIp = sourceIp;
        this->destIp = destIp;
    }
};

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

    // allowed modifiers
    void setSequenceNumber(int);
    void setAckNumber(int);
    void setTCPFlags(int);

    // Checkers
    static bool isSYNPacket(Packet);

    // Actions
    static Packet getSYNPacket(PktData pktData);
    static Packet getSynAckPacket(PktData pktData);

  private:
    IPHeader ipHeader;
    TCPHeader tcpHeader;
    int size;
};

#endif
