#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/ip.h>

using namespace std;

#define PACKET_SIZE 4096

unsigned short checksum(void* vdata, size_t length) {
    char* data = (char*)vdata;
    unsigned long acc = 0;
    for (size_t i = 0; i + 1 < length; i += 2) {
        unsigned short word;
        memcpy(&word, data + i, 2);
        acc += ntohs(word);
        if (acc > 0xffff) {
            acc -= 0xffff;
        }
    }
    if (length & 1) {
        unsigned short word = 0;
        memcpy(&word, data + length - 1, 1);
        acc += ntohs(word);
        if (acc > 0xffff) {
            acc -= 0xffff;
        }
    }
    return htons(~acc);
}

int main() {
    int rawSocket;
    char buffer[PACKET_SIZE];
    struct sockaddr_in destAddress;

    // Create a raw socket
    rawSocket = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (rawSocket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Bind the socket to a specific interface
    struct sockaddr_in clientAddress;
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_addr.s_addr = inet_addr("192.168.1.1");  // Client IP
    clientAddress.sin_port = htons(0);  // Any port

    if (bind(rawSocket, (struct sockaddr*)&clientAddress, sizeof(clientAddress)) < 0) {
        perror("Socket bind failed");
        close(rawSocket);
        return 1;
    }


    // Set up the destination address
    destAddress.sin_family = AF_INET;
    destAddress.sin_port = htons(0);  // No port for raw socket
    destAddress.sin_addr.s_addr = inet_addr("192.168.1.2");  // Server IP

    // Build the IP header
    struct iphdr* ipHeader = (struct iphdr*)buffer;
    ipHeader->ihl = 5;  // Header length (20 bytes)
    ipHeader->version = 4;  // IPv4
    ipHeader->tos = 0;  // Type of service
    ipHeader->tot_len = htons(sizeof(struct iphdr) + strlen("Hello, server!"));  // Total length
    ipHeader->id = htons(54321);  // Identification
    ipHeader->frag_off = 0;  // Fragment offset
    ipHeader->ttl = 64;  // Time to live
    ipHeader->protocol = IPPROTO_RAW;  // Protocol
    ipHeader->check = 0;  // Checksum (initially zero)
    ipHeader->saddr = inet_addr("192.168.1.1");  // Source IP
    ipHeader->daddr = destAddress.sin_addr.s_addr;  // Destination IP

    // Calculate the IP checksum
    ipHeader->check = checksum(ipHeader, sizeof(struct iphdr));

    // Append the payload
    char* payload = buffer + sizeof(struct iphdr);
    strcpy(payload, "Hello, server!");

    // Send the packet
    ssize_t bytesSent = sendto(rawSocket, buffer, sizeof(struct iphdr) + strlen(payload), 0,
                               (struct sockaddr*)&destAddress, sizeof(destAddress));
    if (bytesSent < 0) {
        perror("Error sending packet");
        close(rawSocket);
        return 1;
    }

    cout << "Packet sent successfully!\n";

    // Receive a response (if any)
    char recvBuffer[PACKET_SIZE];
    struct sockaddr_in srcAddress;
    socklen_t srcAddressLen = sizeof(srcAddress);
    ssize_t bytesReceived = recvfrom(rawSocket, recvBuffer, PACKET_SIZE, 0,
                                     (struct sockaddr*)&srcAddress, &srcAddressLen);
    if (bytesReceived < 0) {
        perror("Error receiving packet");
        close(rawSocket);
        return 1;
    }

    cout << "Received a packet of size: " << bytesReceived << " bytes\n";
    cout << "Response payload: " << (recvBuffer + sizeof(struct iphdr)) << "\n";

    close(rawSocket);
    return 0;
}

