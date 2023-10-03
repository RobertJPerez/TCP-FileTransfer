#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8080
#define MAXLINE 1024

typedef struct {
    uint16_t count;
    uint16_t packet_sequence_number;
} PacketHeader;

void receive_file(FILE *fp, int sockfd, int *packet_num);
ssize_t total_received_bytes = 0;

int main() {
    struct sockaddr_in servaddr;
    int sockfd;
    char filename[MAXLINE];
    int packet_num = 1;

    // Create a socket for the client
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    // Get the filename from the user
    printf("Enter the name of the file to be transferred: ");
    fgets(filename, MAXLINE, stdin);
    filename[strcspn(filename, "\n")] = '\0';

    // Create a packet header
    PacketHeader header;
    header.count = htons(strlen(filename));
    header.packet_sequence_number = 0;

    // Send the packet header
    if (send(sockfd, &header, sizeof(header), 0) == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }

    // Send the filename
    send(sockfd, filename, strlen(filename) + 1, 0);

    // Open the output file for writing
    FILE *fp = fopen("out.txt", "wb");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Receive the file from the server
    receive_file(fp, sockfd, &packet_num);

    // Print statistics about the file transfer - works when I modify send_file but breaks packet number???
    printf("File received.\n");
    printf("Number of data packets received: %d\n", packet_num - 1);
    printf("Total received bytes: %ld\n", total_received_bytes);

    fclose(fp);
    close(sockfd);

    return 0;
}

// Function to receive a file from the server - connection refused? wrong port/compile again???? 
void receive_file(FILE *fp, int sockfd, int *packet_num) {
    int n;
    uint16_t packet_num_local;
    char data[MAXLINE] = {0};
    PacketHeader header;
    *packet_num = 1;

    while (1) {
        if (recv(sockfd, &header, sizeof(header), 0) == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        }

        packet_num_local = ntohs(header.packet_sequence_number);
        *packet_num = packet_num_local;
        n = ntohs(header.count);

        if (n == 0) {
            printf("End of Transmission Packet with sequence number %d received with 0 data bytes\n", packet_num_local);
            break;
        }

        if (recv(sockfd, data, n, 0) == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        }

        printf("Packet %d received with %d data bytes\n", packet_num_local, n);
        total_received_bytes += n;
        fwrite(data, sizeof(char), n, fp);
        memset(data, 0, MAXLINE);
    }
}