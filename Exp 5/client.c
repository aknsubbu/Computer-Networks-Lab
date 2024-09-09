#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define PACKET_SIZE 64  

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[PACKET_SIZE * 4] = {0};  // Buffer size modified to accommodate larger packets

    // Creating socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nSocket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/Address not supported\n");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed\n");
        return -1;
    }

    printf("Connected to the server.\n");

    int expected_seq_num = 0;  // Initialize the expected sequence number

    while (1) {
        // Clear the buffer and read the incoming packets
        memset(buffer, 0, sizeof(buffer));
        int valread = read(sock, buffer, sizeof(buffer));

        if (valread <= 0) {
            printf("No more data from server or connection closed.\n");
            break;
        }

        // Process each packet in the buffer
        char *packet = strtok(buffer, "\n");
        while (packet != NULL) {
            if (strcmp(packet, "DONE") == 0) {
                printf("End of transmission received. Exiting.\n");
                close(sock);
                return 0;
            }

            int received_packet_num = atoi(packet + 7);  // Extract packet number after "Packet "

            if (received_packet_num == expected_seq_num) {
                printf("Received: %s\n", packet);

                // Prompt user for acknowledgment
                int ack_num;
                do {
                    printf("Enter ACK for Packet %d: ", expected_seq_num);
                    scanf("%d", &ack_num);

                    // If the entered ACK is correct, send it and move to the next packet
                    if (ack_num == expected_seq_num) {
                        // Send acknowledgment for the received packet
                        snprintf(buffer, PACKET_SIZE, "%d\n", ack_num);
                        send(sock, buffer, strlen(buffer), 0);
                        printf("Sent ACK for Packet %d\n", ack_num);
                        expected_seq_num++;
                    } else {
                        printf("Incorrect ACK. Please enter the correct ACK for Packet %d.\n", expected_seq_num);
                    }
                } while (ack_num != expected_seq_num);
            } else {
                printf("Unexpected packet received. Expected %d but got %d\n", expected_seq_num, received_packet_num);
            }

            packet = strtok(NULL, "\n");  // Move to the next packet
        }
    }

    close(sock);
    return 0;
}

