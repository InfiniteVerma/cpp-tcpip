#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/ip.h>  // IP header structure
#include <cassert>

using namespace std;

#define PACKET_SIZE 4096

int main() {
    int rawSocket;
    char buffer[PACKET_SIZE];
    struct sockaddr_in srcAddress;
    socklen_t srcAddressLen = sizeof(srcAddress);

    // Create a raw socket to listen for IP packets
    rawSocket = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (rawSocket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Loop to receive incoming packets
    while (true) {
        memset(buffer, 0, PACKET_SIZE);

        // Receive a packet
        ssize_t packetSize = recvfrom(rawSocket, buffer, PACKET_SIZE, 0,
                                      (struct sockaddr*)&srcAddress, &srcAddressLen);
        if (packetSize < 0) {
            perror("Error receiving packet");
            close(rawSocket);
            return 1;
        }

        cout << "Received a packet of size: " << packetSize << " bytes\n";

        // Extract IP header
        struct iphdr* ipHeader = (struct iphdr*)buffer;
        cout << "Source IP: " << inet_ntoa(*(in_addr*)&ipHeader->saddr) << "\n";
        cout << "Destination IP: " << inet_ntoa(*(in_addr*)&ipHeader->daddr) << "\n";

        // Extract payload (assuming simple text payload)
        char* payload = buffer + sizeof(struct iphdr);
        cout << "Payload: " << payload << "\n";
    }

    close(rawSocket);
    return 0;
}

