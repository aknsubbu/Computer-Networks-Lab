#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER 1024
#define PORT 8080
#define SA struct sockaddr

void chatFunction(int sockfd){
    char buffer[MAX_BUFFER];
    int n;
    for (;;){
        bzero(buffer,MAX_BUFFER);
        //read and send
        while((buffer[n++]=getchar())!='\n');
        write(sockfd,buffer,sizeof(buffer));
        bzero(buffer,MAX_BUFFER);
        read(sockfd,buffer,sizeof(buffer));
        printf("From client: %s\t To client : ", buffer);
        if(strncmp("exit",buffer,4)==0){
            printf("Server Exit...\n");
            break;
        }

    }
}

int main(){
    int sockfd;
    struct sockaddr_in servaddr;

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
    servaddr.sin_addr.s_addr =  inet_addr("127.0.0.1");

    if ((connect(sockfd,(SA*)&servaddr,sizeof(servaddr))!=0)){
        printf("Error connecting to the server");
        exit(0);
    }
    else{
        printf("Connected to the server");
    }

    chatFunction(sockfd);

    close(sockfd);


}