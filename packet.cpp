#include "packet.h"

#include <arpa/inet.h>

#include <cstring>

Packet::Packet(int sourcePort, int destPort) : tcpHeader(sourcePort, destPort), size(0) {}

Packet::Packet(char *rawPacket, int size) : tcpHeader(0, 0) {
    cout << "Packet constructor, parsing the pkt\n";
    memcpy(&ipHeader, rawPacket, sizeof(IPHeader));
    memcpy(&tcpHeader, rawPacket + sizeof(IPHeader), sizeof(TCPHeader));
    cout << "Memcpy done\n";

    cout << "flags set: " << tcpHeader.data_offset_and_flags << "\n";
    cout << "sequence no: " << tcpHeader.seq_number << "\n";
}

const char *Packet::makePacket() {
    char *payload = new char[65535];

    ipHeader.checksum = getChecksumVal(&ipHeader);

    memcpy(payload, &ipHeader, sizeof(IPHeader));

    char *ptr = payload + sizeof(IPHeader);
    memcpy(ptr, &tcpHeader, sizeof(TCPHeader));

    size = sizeof(IPHeader) + sizeof(TCPHeader);

    cout << "Sending packet of size: " << size << "\n";
    Utils::hexDump(payload, size);

    return payload;
}

void Packet::setSequenceNumber(int seq) { tcpHeader.seq_number = seq; }

void Packet::setAckNumber(int ack) { tcpHeader.ack_number = ack; }

void Packet::setTCPFlags(int flags) { tcpHeader.data_offset_and_flags = flags; }

int Packet::getSize() { return size; }

int Packet::getSeq() { return tcpHeader.seq_number; }

Packet Packet::getSYNPacket(PktData pktData) {
    Packet packet(pktData.localPortNum, pktData.remotePortNum);

    packet.setSequenceNumber(1);
    packet.setTCPFlags((1 << 1));

    packet.ipHeader.source_addr = inet_addr(pktData.sourceIp);
    packet.ipHeader.dest_addr = inet_addr(pktData.destIp);

    return packet;
}

Packet Packet::getSynAckPacket(PktData pktData) {
    cout << __FUNCTION__ << " BEGIN\n";
    Packet packet(pktData.localPortNum, pktData.remotePortNum);

    // set sequence number
    packet.setSequenceNumber(20);

    // set ack bit and pass seq number given by SYN in ack field
    packet.setAckNumber(pktData.nextExpectedSeqNumber);

    packet.setTCPFlags((1 << 1) | (1 << 4));  // set SYN and ACK
    packet.ipHeader.source_addr = inet_addr(pktData.sourceIp);
    packet.ipHeader.dest_addr = inet_addr(pktData.destIp);

    cout << "Sending packet with source ip: " << pktData.sourceIp << "\n";
    cout << "Sending packet with dest ip: " << pktData.destIp << "\n";

    return packet;
}

bool Packet::isSYNPacket(Packet packet) { return (packet.tcpHeader.data_offset_and_flags > 1); }

Packet::~Packet() {}
