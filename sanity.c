#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sockfd;
    struct sockaddr_in addr;
    int backlog = 5; // Number of allowed connections in the queue

    // Create a raw socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    // Bind the socket to an address
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("0.0.0.0"); // Bind to all available interfaces

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        return EXIT_FAILURE;
    }

    printf("Raw socket created and bound successfully\n");

    // Attempt to listen on the socket
    // Note: listen() is not applicable for raw sockets, this is just for demonstration
    if (listen(sockfd, backlog) < 0) {
        perror("Listen failed");
        close(sockfd);
        return EXIT_FAILURE;
    }

    printf("Listening on raw socket\n");

    // Close the socket
    close(sockfd);
    return EXIT_SUCCESS;
}

