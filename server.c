#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/select.h>

int MAX_CONNECTIONS = 20;

void *handle_connection(void *arg) {
    printf("Handling connection\n");
    int connection_socket = *(int *)arg;
    char buffer[sizeof(struct timespec)];
    printf("Reading from socket\n");

    // fd_set set;
    // struct timeval timeout;

    // FD_ZERO(&set);
    // FD_SET(connection_socket, &set);

    // timeout.tv_sec = 0;
    // timeout.tv_usec = 10000;

    int valread;
    /* Loop. Eventually kill the thread ... */ 
    while (1) {
	valread = read(connection_socket, buffer, sizeof(buffer));
    if (valread <= 0) {
        break;
    }
	send(connection_socket, buffer, sizeof(buffer), 0);
    }

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
    pthread_t threads[MAX_CONNECTIONS];

    if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        printf("Error binding\n");
        exit(0);
    }

    if (listen(sockfd, MAX_CONNECTIONS) < 0) {
        printf("Error listening\n");
        exit(0);
    }

    int i = 0;

    while (1) {
        printf("Value of i: %d\n", i);
        if (i >= MAX_CONNECTIONS) {
            printf("Max number of connections reached\n");
            break;
        }
        int *connection_socket = malloc(sizeof(int));
        *connection_socket = accept(sockfd, (struct sockaddr *)&address, &address_length);
        if (*connection_socket < 0) {
            printf("Error accepting connection\n");
            free(connection_socket);
        }

        int thread_status = pthread_create(&threads[i], NULL, handle_connection, connection_socket);
        if (thread_status != 0) {
            printf("Error creating thread\n");
            free(connection_socket);
        } else {
            pthread_detach(threads[i]);
        }

        i++;
    }

    close(sockfd);
    return 0;
}
