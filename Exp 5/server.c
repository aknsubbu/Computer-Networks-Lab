#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <time.h>

#define PORT 8080
#define TOTAL_PACKETS 15
#define PACKET_SIZE 64  
#define TIMEOUT 5  // 5 seconds timeout for ACK

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[PACKET_SIZE] = {0};
    fd_set readfds;
    struct timeval timeout;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is waiting for a connection...\n");

    // Accept the incoming connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connection established with client.\n");

    int seq_num = 0;  // Sequence number to send

    while (seq_num < TOTAL_PACKETS) {
        int ack_received = 0;

        while (!ack_received) {
            // Send the packet
            snprintf(buffer, PACKET_SIZE, "Packet %d\n", seq_num);
            send(new_socket, buffer, strlen(buffer), 0);
            printf("Sent: %s", buffer);

            // Initialize the file descriptor set
            FD_ZERO(&readfds);
            FD_SET(new_socket, &readfds);

            // Set the timeout value
            timeout.tv_sec = TIMEOUT;
            timeout.tv_usec = 0;

            // Wait for an acknowledgment with a timeout
            int activity = select(new_socket + 1, &readfds, NULL, NULL, &timeout);

            if (activity > 0) {
                // Receive acknowledgment from the client
                memset(buffer, 0, PACKET_SIZE);
                int valread = read(new_socket, buffer, PACKET_SIZE);
                if (valread <= 0) {
                    printf("No ACK received or connection closed. Exiting.\n");
                    close(new_socket);
                    close(server_fd);
                    return 0;
                }

                buffer[valread] = '\0';
                int ack_num = atoi(buffer);  // Convert ACK to integer
                printf("Received ACK for Packet %d\n", ack_num);

                // Check if the acknowledgment is for the current packet
                if (ack_num == seq_num) {
                    ack_received = 1;  // ACK received, move to the next packet
                } else {
                    printf("Unexpected ACK received. Expected %d but got %d\n", seq_num, ack_num);
                }
            } else if (activity == 0) {
                // Timeout occurred, resend the packet
                printf("Timeout occurred. Resending Packet %d\n", seq_num);
            }
        }

        seq_num++;  // Move to the next packet
    }

    // Send an end-of-transmission message
    snprintf(buffer, PACKET_SIZE, "DONE\n");
    send(new_socket, buffer, strlen(buffer), 0);
    printf("Sent end-of-transmission message.\n");

    close(new_socket);
    close(server_fd);
    return 0;
}