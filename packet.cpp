#include "packet.h"
#include <cstring>

Packet::Packet(int sourcePort, int destPort)
    : tcpHeader(sourcePort, destPort), size(0) {}

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

    tcpHeader.seq_number = 1;
    tcpHeader.data_offset_and_flags = (1 << 1); // setting SYN flag

    char *ptr = payload + sizeof(IPHeader);
    memcpy(ptr, &tcpHeader, sizeof(TCPHeader));

    // int size = sizeof(IPHeader) + sizeof(TCPHeader) + 10;
    size = sizeof(IPHeader) + sizeof(TCPHeader);
    cout << "Sending packet of size: " << size << "\n";
    Utils::hexDump(payload, size);

    return payload;
}

int Packet::getSize() { return size; }

bool Packet::isSYN() {
    if (tcpHeader.data_offset_and_flags > 1)
        return true;

    return false;
}

int Packet::getSeq() { return tcpHeader.seq_number; }

Packet::~Packet() {}
