#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX 80
#define PORT 3000
#define SA struct sockaddr

void send_request(int sockfd, struct sockaddr_in *servaddr) {
while(1){
    char buffer[MAX];
    char response[MAX];
    socklen_t len = sizeof(*servaddr);

    // Get URL from user
    printf("Enter the URL: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline character

    if (strcmp(buffer,"exit")==0){
break;
}
else{       // Send URL to server
    sendto(sockfd, buffer, strlen(buffer), 0, (SA*)servaddr, len);
    printf("URL Sent\n");
    // Receive response from server
    recvfrom(sockfd, response, sizeof(response), 0, NULL, NULL);
    printf("Server response: %s\n", response);
}
}
}             

int main() {
    int sockfd;
    struct sockaddr_in servaddr;

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
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // Send request and receive response
    send_request(sockfd, &servaddr);

    close(sockfd);
    return 0;
}

