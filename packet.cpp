#include "packet.h"
#include <cstring>

Packet::Packet(int sourcePort, int destPort)
{
    tcpHeader.source_port = sourcePort;
    tcpHeader.dest_port = destPort;
}

const char*
Packet::makePacket()
{
    const char* buffer = new char[16];
    memcpy(&buffer, &tcpHeader.source_port, sizeof(UINT16));
    return buffer;
}

Packet::~Packet()
{
}
