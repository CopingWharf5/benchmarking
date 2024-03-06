#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <number of messages> <message size in bytes> <addr>\n", argv[0]);
        return 1;
    }
    int msg_count = atoi(argv[1]);
    int msg_size = atoi(argv[2]);
    char *addr = argv[3];
    int status = 0;
    int sum = 0;
    long bytes_sent = 0;
    float latency[msg_count];

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Error creating socket\n");
        exit(0);
    }

    // Assigning port and IP
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);

    // Convert IP to binary
    if (inet_pton(AF_INET, addr, &server_address.sin_addr) <= 0) {
        printf("Invalid address\n");
        exit(0);
    }
    
    // Connect to server
    status = connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));
    if (status == -1) {
        printf("Error connecting to server\n");
        exit(0);
    }

    // Send messages
    char msg[msg_size + sizeof(struct timespec)];
    char recv_msg[sizeof(struct timespec)];
    struct timespec start_time;
    struct timespec end_time;
    struct timespec total_start, total_end;
    float total_time = 0.0;
    for (int j = sizeof(struct timespec); j < msg_size+ sizeof(struct timespec); j++) {
        msg[j] = 'A'; 
    }
    clock_gettime(CLOCK_REALTIME, &total_start);

    for (int i = 0; i < msg_count; i++) {
        clock_gettime(CLOCK_REALTIME, &start_time);
        memcpy(msg, &start_time, sizeof(start_time));
        
        send(sockfd, msg, sizeof(msg), 0);
        bytes_sent += sizeof(msg);
        if (recv(sockfd, recv_msg, sizeof(recv_msg), 0) < 0)
            break;
        else {
            clock_gettime(CLOCK_REALTIME, &end_time);
            latency[i] = (float) ((end_time.tv_sec - start_time.tv_sec) * 1000) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000.0;
            sum += latency[i];
        }
    }
    clock_gettime(CLOCK_REALTIME, &total_end);
    total_time = (float) ((total_end.tv_sec - total_start.tv_sec) * 1000) + (total_end.tv_nsec - total_start.tv_nsec) / 1000000.0;

    printf("Average latency: %f ms\n", (float) (sum / msg_count));
    printf("Total time: %f ms\n", total_time);
    printf("Throughput: %f MB/s\n", ((bytes_sent / 1000000) / total_time)  * 1000);

    close(sockfd); 
    return 0;
}
