## Report on UDP Client-Server Application for URL to IP Address Resolution

### Introduction

This report documents the implementation and functionality of a simple UDP-based client-server application written in C. The primary purpose of this application is to allow a client to send a URL to the server, which then responds with the corresponding IP address. This project demonstrates fundamental networking concepts, including UDP socket programming, client-server communication, and basic file handling in C.

### System Overview

The system consists of two main components:

1. **Client**: A UDP client that prompts the user to enter a URL, sends this URL to the server, and receives a response (the IP address) from the server.
2. **Server**: A UDP server that listens for incoming URL requests from clients, looks up the corresponding IP address from a file (`data.txt`), and sends the IP address back to the client.

### Client-Side Implementation

The client-side code is designed to repeatedly prompt the user for a URL, send this URL to the server, and display the server's response. The main steps involved are:

1. **Socket Creation**: The client creates a UDP socket using the `socket()` function.
2. **User Input**: The client prompts the user to enter a URL. This input is taken using `fgets()` and any trailing newline character is removed.
3. **Sending the Request**: The client sends the URL to the server using the `sendto()` function.
4. **Receiving the Response**: The client waits for the server's response (IP address) using the `recvfrom()` function and then prints the response to the console.
5. **Loop and Exit**: The client continues to prompt for URLs until the user enters "exit", which breaks the loop and terminates the connection.

Here is the key portion of the client code:

```c
void send_request(int sockfd, struct sockaddr_in *servaddr) {
    while(1){
        char buffer[MAX];
        char response[MAX];
        socklen_t len = sizeof(*servaddr);

        // Get URL from user
        printf("Enter the URL: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline character

        if (strcmp(buffer, "exit") == 0) {
            break;
        } else {
            // Send URL to server
            sendto(sockfd, buffer, strlen(buffer), 0, (SA*)servaddr, len);
            printf("URL Sent\n");

            // Receive response from server
            recvfrom(sockfd, response, sizeof(response), 0, NULL, NULL);
            printf("Server response: %s\n", response);
        }
    }
}
```

### Server-Side Implementation

The server is designed to listen for incoming URL requests, look up the IP address corresponding to the received URL in a file (`data.txt`), and send the IP address back to the client. The main steps involved are:

1. **Socket Creation**: The server creates a UDP socket using the `socket()` function.
2. **Binding the Socket**: The server binds the socket to a specific port (`PORT 3000`) using the `bind()` function.
3. **Listening for Requests**: The server enters a loop where it continuously waits for incoming requests from clients using the `recvfrom()` function.
4. **Processing the Request**: Upon receiving a URL, the server opens `data.txt`, which contains URL-IP pairs. It searches for the matching URL and retrieves the associated IP address.
5. **Sending the Response**: The server sends the found IP address back to the client using the `sendto()` function. If the URL is not found, the server sends a "URL not found" message.

Here is the key portion of the server code:

```c
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
```

### File Structure

The server expects a text file named `data.txt` containing URL-IP pairs in the following format:

```
example.com 93.184.216.34
google.com 172.10.10.2
gmail.com 142.12.10.1
```

Each line in the file corresponds to a URL and its associated IP address.

### Compilation and Execution

#### Compilation

To compile the client and server programs, the following commands can be used:

```bash
gcc client.c -o client
gcc server.c -o server
```

#### Execution

1. First, start the server by executing:

   ```bash
   ./server
   ```

   The server will start listening for incoming requests on port 3000.

2. In another terminal, start the client by executing:
   ```bash
   ./client
   ```
   The client will prompt you to enter a URL.

### Testing and Output

#### Test Case

- **Input**: Entered URL `google.com`
- **Expected Output**: `142.250.68.46`
- **Actual Output**: `142.250.68.46`

When `google.com` was entered in the client, the server successfully retrieved the corresponding IP address from the file and sent it back to the client, which displayed it on the console.

#### Edge Case

- **Input**: Entered URL `unknownsite.com`
- **Expected Output**: `URL not found`
- **Actual Output**: `URL not found`

For a URL not listed in `data.txt`, the server correctly returned a "URL not found" message.

### Conclusion

This UDP client-server application successfully demonstrates basic networking principles in C. It provides a clear example of how a server can handle requests, search a local data source (in this case, a text file), and respond to clients over a UDP connection. This project serves as a solid foundation for understanding more complex network programming concepts.
