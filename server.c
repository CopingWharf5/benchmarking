#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>

int main(int argc, char *argv[]){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Error creating socket\n");
        exit(0);
    }

    struct sockaddr_in address;
    socklen_t address_length = sizeof(address);
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        printf("Error binding\n");
        exit(0);
    }

    if (listen(sockfd, 3) < 0) {
        printf("Error listening\n");
        exit(0);
    }

    if ((connection_socket = accept(sockfd, (struct sockaddr *)&address, &address_length)) < 0) {
        printf("Error accepting\n");
        exit(0);
    }

}