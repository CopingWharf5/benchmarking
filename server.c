#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <pthread.h>

void *handle_connection(void *arg) {
    printf("Handling connection\n");
    int connection_socket = *(int *)arg;
    char buffer[sizeof(struct timespec)];
    printf("Reading from socket\n");
    int valread = read(connection_socket, buffer, sizeof(struct timespec));
    send(connection_socket, buffer, sizeof(buffer), 0);
    close(connection_socket);
    printf("Connection closed\n");
    free(arg);
    printf("Exiting thread\n");
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
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
    pthread_t threads[20];

    if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        printf("Error binding\n");
        exit(0);
    }

    if (listen(sockfd, 20) < 0) {
        printf("Error listening\n");
        exit(0);
    }

    int i = 0;

    while (1) {
        printf("Value of i: %d\n", i);
        if (i >= 20) {
            printf("Max number of connections reached\n");
            break;
        }
        int *connection_socket = malloc(sizeof(int));
        *connection_socket = accept(sockfd, (struct sockaddr *)&address, &address_length);
        if (*connection_socket < 0) {
            printf("Error accepting connection\n");
            free(connection_socket);
        }

        pthread_create(&threads[i], NULL, handle_connection, connection_socket);
        i++;
    }

    close(sockfd);
    return 0;
}