#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>

int main(int argc, char *argv[]){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Error creating socket\n");
        exit(0);
    }

    struct sockaddr_in address;
    socklen_t address_length = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    address.sin_addr.s_addr = INADDR_ANY;
    int connection_socket;

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

    while (1) {
        char buffer[sizeof(struct timespec)] = {0};
        int valread = read(connection_socket, buffer, 1024);
        printf("%s\n", buffer);
        send(connection_socket, buffer, sizeof(buffer), 0);
        }
    
    close(connection_socket);
    close(sockfd);
    return 0;
}