#include "packet.h"

#include <arpa/inet.h>

#include <cassert>
#include <cstring>

#include "ip.h"

Packet::Packet() : tcpHeader(0, 0), headerSize(0), payloadSize(0), payload(nullptr) {}

Packet::Packet(int sourcePort, int destPort)
    : tcpHeader(sourcePort, destPort), headerSize(0), payloadSize(0), payload(nullptr) {}

Packet::~Packet() {
    if (!payload) {
        delete[] payload;
    }
}

Packet::Packet(const char *rawPacket, int size) : tcpHeader(0, 0) {
    memcpy(&ipHeader, rawPacket, sizeof(IPHeader));
    memcpy(&tcpHeader, rawPacket + sizeof(IPHeader), sizeof(TCPHeader));

    if (size != sizeof(IPHeader) + sizeof(TCPHeader)) {
        LOG(__FUNCTION__, " contains data payload as well. TODO");
        this->payloadSize = size - (sizeof(IPHeader) + sizeof(TCPHeader));
        this->payload = new char[this->payloadSize];
        memcpy(this->payload, rawPacket + sizeof(IPHeader) + sizeof(TCPHeader), this->payloadSize);
    } else {
        this->payloadSize = 0;
        this->payload = nullptr;
    }
    this->headerSize = size;
}

const char *Packet::makePacket() {
    // LOG(__FUNCTION__, " BEGIN");
    char *buffer = new char[65535];

    ipHeader.checksum = getChecksumVal(&ipHeader);

    memcpy(buffer, &ipHeader, sizeof(IPHeader));
    char *ptr = buffer + sizeof(IPHeader);

    memcpy(ptr, &tcpHeader, sizeof(TCPHeader));

    this->headerSize = sizeof(IPHeader) + sizeof(TCPHeader);

    if (this->payload) {
        ptr += sizeof(TCPHeader);
        memcpy(ptr, this->payload, payloadSize);
    }

    return buffer;
}

void Packet::setSeq(int seq) { tcpHeader.seq_number = seq; }

void Packet::setAckNumber(int ack) { tcpHeader.ack_number = ack; }

void Packet::setTCPFlags(int flags) { tcpHeader.data_offset_and_flags = flags; }

int Packet::getSize() { return headerSize + payloadSize; }

int Packet::getSeq() { return tcpHeader.seq_number; }

int Packet::getAck() { return tcpHeader.ack_number; }

Packet Packet::constructDataPacket(PktData pktData, const char *payload, int size) {
    LOG(__FUNCTION__, " BEGIN");
    Packet packet(pktData.localPortNum, pktData.remotePortNum);

    packet.setSeq(pktData.seqNumber);
    packet.ipHeader.source_addr = inet_addr(pktData.sourceIp);
    packet.ipHeader.dest_addr = inet_addr(pktData.destIp);

    packet.addPayload(payload, size);

    return packet;
}

void Packet::addPayload(const char *payload, int size) {
    LOG(__FUNCTION__, " BEGIN");
    if (this->payload == nullptr) {
        this->payload = new char[size];
        memcpy(this->payload, payload, size);
        this->payloadSize = size;
        LOG(__FUNCTION__, " added payload size of: ", size);
    } else {
        LOG(__FUNCTION__, " ERROR this->payload also has some memory");
        assert(0);
    }
}

Packet Packet::getSYNPacket(PktData pktData) {
    LOG(__FUNCTION__, " BEGIN");
    Packet packet(pktData.localPortNum, pktData.remotePortNum);

    packet.setSeq(pktData.seqNumber);
    packet.setTCPFlags((1 << 1));

    packet.ipHeader.source_addr = inet_addr(pktData.sourceIp);
    packet.ipHeader.dest_addr = inet_addr(pktData.destIp);

    return packet;
}

Packet Packet::getSynAckPacket(PktData pktData) {
    LOG(__FUNCTION__, " BEGIN\n");
    Packet packet(pktData.localPortNum, pktData.remotePortNum);

    // set sequence number
    packet.setSeq(pktData.seqNumber);

    // set ack bit and pass seq number given by SYN in ack field
    packet.setAckNumber(pktData.ackNumber);

    packet.setTCPFlags((1 << 1) | (1 << 4));  // set SYN and ACK
    packet.ipHeader.source_addr = inet_addr(pktData.sourceIp);
    packet.ipHeader.dest_addr = inet_addr(pktData.destIp);
    return packet;
}

bool Packet::isSynPacket(Packet packet) { return packet.isSYNSet(); }

bool Packet::isSynAckPacket(Packet packet) { return packet.isSYNSet() && packet.isACKSet(); }

bool Packet::isRstPacket(Packet packet) { return packet.isRSTSet(); }

bool Packet::isNotRstPacket(Packet packet) { return !packet.isRSTSet(); }

bool Packet::isAckPacket(Packet packet) {
    return packet.isACKSet();
}  //{ return (packet.tcpHeader.data_offset_and_flags > 4); }

Packet Packet::getAckPacket(PktData pktData) {
    LOG(__FUNCTION__, " BEGIN\n");
    Packet packet(pktData.localPortNum, pktData.remotePortNum);

    packet.ipHeader.source_addr = inet_addr(pktData.sourceIp);
    packet.ipHeader.dest_addr = inet_addr(pktData.destIp);

    packet.setTCPFlags((1 << 4));  // ACK
    packet.setAckNumber(pktData.ackNumber);
    packet.setSeq(pktData.seqNumber);

    return packet;
}

Packet Packet::getRSTPacket(PktData pktData) {
    LOG(__FUNCTION__, " BEGIN\n");
    Packet packet(pktData.localPortNum, pktData.remotePortNum);

    packet.ipHeader.source_addr = inet_addr(pktData.sourceIp);
    packet.ipHeader.dest_addr = inet_addr(pktData.destIp);

    packet.setSeq(pktData.ackNumber);
    packet.setTCPFlags((1 << 1) | (1 << 2));

    return packet;
}

bool Packet::isRSTSet() { return (tcpHeader.data_offset_and_flags & (1 << 2)); }

bool Packet::isACKSet() { return (tcpHeader.data_offset_and_flags & (1 << 4)); }

bool Packet::isSYNSet() { return (tcpHeader.data_offset_and_flags & (1 << 1)); }

void Packet::printPacket() {
    LOG(__FUNCTION__);
    LOG("BEGIN header size: ", headerSize);
    LOG("      payload size: ", payloadSize);

    LOG("IP Header");
    LOG("  idl: ", ipHeader.idl);
    LOG("  version: ", ipHeader.version);
    LOG("  type_of_service: ", ipHeader.type_of_service);
    LOG("  total_length: ", ipHeader.total_length);
    LOG("  identification: ", ipHeader.identification);
    LOG("  fragment_offset: ", ipHeader.fragment_offset);
    LOG("  ttl: ", ipHeader.ttl);
    LOG("  protocol: ", ipHeader.protocol);
    LOG("  checksum: ", ipHeader.checksum);
    LOG("  source_addr: ", ipHeader.source_addr);
    LOG("  dest_addr: ", ipHeader.dest_addr);

    LOG("TCP Header");
    LOG("  source_port: ", tcpHeader.source_port);
    LOG("  dest_port: ", tcpHeader.dest_port);
    LOG("  seq_number: ", tcpHeader.seq_number);
    LOG("  ack_number: ", tcpHeader.ack_number);
    prettyPrintFlags();
    LOG("  window: ", tcpHeader.window);
    LOG("  checksum: ", tcpHeader.checksum);
    LOG("  urgent_pointer: ", tcpHeader.urgent_pointer);
    LOG(__FUNCTION__, " END");
}

void Packet::prettyPrintFlags() {
    string res = "";

    if (isACKSet()) res += "ACK, ";

    if (isSYNSet()) res += "SYN, ";

    if (isRSTSet()) res += "RST, ";

    LOG("  data_offset_and_flags: ", res);
}
