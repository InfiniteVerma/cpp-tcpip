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
    struct sockaddr_in srcAddress;
    socklen_t srcAddressLen = sizeof(srcAddress);

    // Create a raw socket
    rawSocket = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (rawSocket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Bind the socket to a specific interface
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("192.168.1.2");  // Server IP
    serverAddress.sin_port = htons(0);  // Any port

    if (bind(rawSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Socket bind failed");
        close(rawSocket);
        return 1;
    }


    // Loop to receive packets
    while (true) {
        memset(buffer, 0, PACKET_SIZE);

        ssize_t packetSize = recvfrom(rawSocket, buffer, PACKET_SIZE, 0,
                                      (struct sockaddr*)&srcAddress, &srcAddressLen);
        if (packetSize < 0) {
            perror("Error receiving packet");
            close(rawSocket);
            return 1;
        }

        cout << "Received a packet of size: " << packetSize << " bytes\n";

        struct iphdr* ipHeader = (struct iphdr*)buffer;
        cout << "Source IP: " << inet_ntoa(*(in_addr*)&ipHeader->saddr) << "\n";
        cout << "Destination IP: " << inet_ntoa(*(in_addr*)&ipHeader->daddr) << "\n";

        char* payload = buffer + sizeof(struct iphdr);
        cout << "Payload: " << payload << "\n";

        // Send a response back to the client
        char* responsePayload = "Hello, client!";
        ipHeader->saddr = inet_addr("192.168.1.2");  // Server IP
        ipHeader->daddr = srcAddress.sin_addr.s_addr;  // Client IP
        ipHeader->tot_len = htons(sizeof(struct iphdr) + strlen(responsePayload));

        // Calculate checksum for the new packet
        ipHeader->check = 0;
        ipHeader->check = checksum(ipHeader, sizeof(struct iphdr));

        strcpy(buffer + sizeof(struct iphdr), responsePayload);

        ssize_t bytesSent = sendto(rawSocket, buffer, sizeof(struct iphdr) + strlen(responsePayload), 0,
                                   (struct sockaddr*)&srcAddress, srcAddressLen);
        if (bytesSent < 0) {
            perror("Error sending response");
            close(rawSocket);
            return 1;
        }

        cout << "Response sent to client\n";
        break;
    }

    close(rawSocket);
    return 0;
}

