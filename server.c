#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Define constants
//default port
#define PORT 8080
#define MAXLINE 1024

// Define packet header structure
typedef struct {
    uint16_t count;
    uint16_t packet_sequence_number;
} PacketHeader;

// Function prototypes
void send_file(FILE *fp, int sockfd, int *packet_num);
ssize_t total_sent_bytes = 0;

// Main function
int main() {
    // Define variables
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    int listenfd, connfd;
    int packet_num = 1;

    // Create socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Bind socket to the server address
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections - works when broken
    if (listen(listenfd, 10) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept a connection from the client - works 
    len = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);
    if (connfd == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Client connected successfully
    printf("A client has connected.\n");
    memset(buffer, 0, MAXLINE);

    // Receive file request from the client
    recv(connfd, buffer, MAXLINE, 0);
    printf("File request: %s\n", buffer);
    FILE *fp = fopen(buffer, "rb");

    // Check if file opened successfully - working once 
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Send the requested file - working once...why?
    send_file(fp, connfd, &packet_num);
    printf("File sent.\n");
    printf("Number of data packets transmitted: %d\n", packet_num - 1);
    printf("Total sent bytes: %ld\n", total_sent_bytes);

    // Cleanup - working
    fclose(fp);
    close(connfd);
    close(listenfd);

    return 0;
}

// Send file function - working once. gets stuck on packet 0 
void send_file(FILE *fp, int sockfd, int *packet_num) {
    int n;
    *packet_num = 1;
    char data[MAXLINE] = {0};
    uint16_t packet_seq_num = 1;
    PacketHeader header;

    while ((n = fread(data, sizeof(char), MAXLINE, fp)) > 0) {
        header.count = htons(n);
        header.packet_sequence_number = htons(packet_seq_num);
        packet_seq_num++; // Increment packet sequence number - why not work when modify?

        if (send(sockfd, &header, sizeof(header), 0) == -1) {
            perror("send");
            exit(EXIT_FAILURE);
        }

        if (send(sockfd, data, n, 0) == -1) {
            perror("send");
            exit(EXIT_FAILURE);
        }

        printf("Packet %d transmitted with %d data bytes\n", packet_seq_num - 1, n);
        total_sent_bytes += n;
        *packet_num = packet_seq_num;
        memset(data, 0, MAXLINE);
    }

    header.count = htons(0);
    header.packet_sequence_number = htons(packet_seq_num);

    send(sockfd, &header, sizeof(header), 0);
    printf("End of Transmission Packet with sequence number %d transmitted with 0 data bytes\n", packet_seq_num);
}
