#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_BUFFER 1024
#define PORT 8080
#define SA struct sockaddr

void chatFunction(int sockfd){
    char buffer[MAX_BUFFER];
    int n;
    for(;;){
        //first read from clinet
        read(sockfd, buffer, sizeof(buffer));
        printf("From client: %s\t To client : ", buffer);
        bzero(buffer, sizeof(buffer));
        n = 0;
        while((buffer[n++]=getchar())!='\n');
        write(sockfd, buffer, sizeof(buffer));
        if(strncmp("exit", buffer, 4)==0){
            printf("Server Exit...\n");
            break;
        }

    }
}


int main(){
    int sockfd,client_fd;
    struct sockaddr_in servaddr,client;
    socklen_t len;

    //create the socket
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd<0){
        printf("Error creating the socket ");
        exit(0);
    }
    else{
        printf("Socket created successfully");
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr =  htonl(INADDR_ANY);

    if ((bind(sockfd,(SA*)&servaddr,sizeof(servaddr))!=0)){
        printf("Errror binding to a port");
        exit(0);
    }
    else{
        printf("Socket binded succcesfully");
    }

    if ((listen(sockfd,5))!=0){
        printf("Listen Failed");
        exit(0);
    }
    else{
        printf("Listening");
    }

    len = sizeof(client);

    client_fd = connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
    if (client_fd<0){
        printf("Connection not accepted");
        exit(0);
    }

    chatFunction(sockfd);

    close(sockfd);


}