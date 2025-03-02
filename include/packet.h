#ifndef __RFC793_PACKET__
#define __RFC793_PACKET__

#include "ip.h"
#include "tcp.h"

struct PktData {
    int localPortNum;
    int remotePortNum;
    int ackNumber;  // 1 + seq get from older pkt
    int seqNumber;
    const char *sourceIp;
    const char *destIp;

    PktData(int localP, int remoteP, int ackNo, int seqNo, const char *sourceIp, const char *destIp) {
        LOG("PktData constructor called, ackNo: ", ackNo, " seqNo: ", seqNo);
        localPortNum = localP;
        remotePortNum = remoteP;
        ackNumber = ackNo;
        seqNumber = seqNo;
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
    Packet();
    Packet(const char *rawPacket, int size);
    ~Packet();

    const char *makePacket();
    void printPacket();
    void prettyPrintFlags();

    int getSize();
    int getSeq();
    int getAck();
    bool isACKSet();
    bool isRSTSet();
    bool isSYNSet();

    // allowed modifiers
    void setSeq(int);
    void setAckNumber(int);
    void setTCPFlags(int);

    // Checkers
    static bool isSynPacket(Packet);
    static bool isSynAckPacket(Packet);
    static bool isAckPacket(Packet);
    static bool isRstPacket(Packet);
    static bool isNotRstPacket(Packet);

    // Actions
    static Packet getSYNPacket(PktData pktData);
    static Packet getSynAckPacket(PktData pktData);
    static Packet getAckPacket(PktData pktData);
    static Packet getRSTPacket(PktData pktData);

    static Packet constructDataPacket(PktData pktData, const char *payload, int size);

   private:
    void addPayload(const char *, int);

    IPHeader ipHeader;
    TCPHeader tcpHeader;
    int headerSize;
    int payloadSize;
    char *payload;
};

#endif
