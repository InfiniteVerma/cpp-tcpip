#include "packet.h"

#include <arpa/inet.h>

#include <cstring>

#include "ip.h"

Packet::Packet(int sourcePort, int destPort) : tcpHeader(sourcePort, destPort), size(0) {}

Packet::Packet(char *rawPacket, int size) : tcpHeader(0, 0) {
    LOG("Packet constructor, parsing the pkt\n");
    memcpy(&ipHeader, rawPacket, sizeof(IPHeader));
    memcpy(&tcpHeader, rawPacket + sizeof(IPHeader), sizeof(TCPHeader));
    LOG("Memcpy done\n");

    LOG("flags set: ", tcpHeader.data_offset_and_flags, "\n");
    LOG("sequence no: ", tcpHeader.seq_number, "\n");
}

const char *Packet::makePacket() {
    LOG(__FUNCTION__, " BEGIN");
    char *payload = new char[65535];

    ipHeader.checksum = getChecksumVal(&ipHeader);

    memcpy(payload, &ipHeader, sizeof(IPHeader));
    LOG(__FUNCTION__, " copied ip header of size: ", sizeof(IPHeader));
    Utils::hexDump(payload, sizeof(IPHeader));

    char *ptr = payload + sizeof(IPHeader);
    memcpy(ptr, &tcpHeader, sizeof(TCPHeader));
    LOG(__FUNCTION__, " copied tcp header of size: ", sizeof(tcpHeader));

    size = sizeof(IPHeader) + sizeof(TCPHeader);

    LOG(__FUNCTION__, " Sending packet of total size: ", size);
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

    packet.setSequenceNumber(pktData.seqNumber);
    packet.setTCPFlags((1 << 1));

    packet.ipHeader.source_addr = inet_addr(pktData.sourceIp);
    packet.ipHeader.dest_addr = inet_addr(pktData.destIp);

    return packet;
}

Packet Packet::getSynAckPacket(PktData pktData) {
    LOG(__FUNCTION__, " BEGIN\n");
    Packet packet(pktData.localPortNum, pktData.remotePortNum);

    // set sequence number
    packet.setSequenceNumber(20);

    // set ack bit and pass seq number given by SYN in ack field
    packet.setAckNumber(pktData.ackNumber);

    packet.setTCPFlags((1 << 1) | (1 << 4));  // set SYN and ACK
    packet.ipHeader.source_addr = inet_addr(pktData.sourceIp);
    packet.ipHeader.dest_addr = inet_addr(pktData.destIp);

    LOG("Sending packet with source ip: ", pktData.sourceIp, "\n");
    LOG("Sending packet with dest ip: ", pktData.destIp, "\n");

    return packet;
}

bool Packet::isSynPacket(Packet packet) { return (packet.tcpHeader.data_offset_and_flags > 1); }

bool Packet::isSynAckPacket(Packet packet) {
    int flags = packet.tcpHeader.data_offset_and_flags;

    return (flags > 1) & (flags > 4);
}

bool Packet::isAckPacket(Packet packet) { return (packet.tcpHeader.data_offset_and_flags > 4); }

Packet Packet::getAckPacket(PktData pktData) {
    LOG(__FUNCTION__, " BEGIN\n");
    Packet packet(pktData.localPortNum, pktData.remotePortNum);

    packet.ipHeader.source_addr = inet_addr(pktData.sourceIp);
    packet.ipHeader.dest_addr = inet_addr(pktData.destIp);
    LOG("Sending packet with source ip: ", pktData.sourceIp, "\n");
    LOG("Sending packet with dest ip: ", pktData.destIp, "\n");

    packet.setTCPFlags((1 << 4));  // ACK
    packet.setAckNumber(pktData.ackNumber);

    return packet;
}

Packet::~Packet() {}
