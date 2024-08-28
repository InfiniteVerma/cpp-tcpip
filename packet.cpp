#include "packet.h"
#include <cstring>

Packet::Packet(int sourcePort, int destPort)
    : tcpHeader(sourcePort, destPort) {}

const char *Packet::makePacket() {
    const char *buffer = new char[16];
    memcpy(&buffer, &tcpHeader.source_port, sizeof(UINT16));
    return buffer;
}

Packet::~Packet() {}
