#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX 80
#define PORT 3000
#define SA struct sockaddr

// Function to look up the IP address for the given URL
void handle_request(int sockfd, struct sockaddr_in *client_addr, socklen_t client_len) {
    char buffer[MAX];
    char response[MAX];
    FILE *fp;
    char url[MAX];
    char ip[MAX];
    socklen_t len = sizeof(*client_addr);

    // Receive URL from client
    ssize_t recv_len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (SA*)client_addr, &client_len);
    if (recv_len < 0) {
        perror("Receive failed");
        return;
    }
    buffer[recv_len] = '\0';  // Null-terminate the received string

    // Debugging: Print the received URL
    printf("Received URL: %s\n", buffer);

    // Open the text file containing URL-IP pairs
    fp = fopen("data.txt", "r");
    if (fp == NULL) {
        perror("Unable to open file");
        snprintf(response, sizeof(response), "Server error");
        sendto(sockfd, response, strlen(response), 0, (SA*)client_addr, len);
        return;
    }

    // Initialize response with "URL not found" message
    snprintf(response, sizeof(response), "URL not found");

    // Read the file line by line
    while (fscanf(fp, "%s %s", url, ip) != EOF) {
        if (strcmp(buffer, url) == 0) {
            snprintf(response, sizeof(response), "%s", ip);
            break;
        }
    }

    fclose(fp);

    // Debugging: Print the response to be sent
    printf("Sending response: %s\n", response);

    // Send the response to the client
    sendto(sockfd, response, strlen(response), 0, (SA*)client_addr, len);
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cli;
    socklen_t len = sizeof(cli);

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("UDP socket created\n");

    // Initialize server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(sockfd, (SA*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("UDP server listening on port %d\n", PORT);

    // Handle incoming requests
    while (1) {
        handle_request(sockfd, &cli, len);
    }

    close(sockfd);
    return 0;
}

