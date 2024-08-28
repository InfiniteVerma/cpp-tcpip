#include "packet.h"
#include <cstring>

Packet::Packet(int sourcePort, int destPort)
    : tcpHeader(sourcePort, destPort), size(0) {}

const char *Packet::makePacket() {
    char *payload = new char[65535];

    ipHeader.checksum = getChecksumVal(&ipHeader);

    memcpy(payload, &ipHeader, sizeof(IPHeader));

    tcpHeader.seq_number = 111;
    tcpHeader.data_offset_and_flags = (1 << 1); // setting SYN flag

    char *ptr = payload + sizeof(IPHeader);
    memcpy(ptr, &tcpHeader, sizeof(TCPHeader));

    // int size = sizeof(IPHeader) + sizeof(TCPHeader) + 10;
    size = sizeof(IPHeader) + sizeof(TCPHeader);
    cout << "Sending packet of size: " << size << "\n";
    Utils::hexDump(payload, size);

    return payload;
}

Packet::~Packet() {}
