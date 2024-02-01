#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER_SIZE 80
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9129

int createSocket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

void setupServerAddress(struct sockaddr_in *address) {
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = inet_addr(SERVER_IP);
    address->sin_port = htons(SERVER_PORT);
}

void connectToServer(int sockfd, struct sockaddr *address, socklen_t len) {
    printf("Connecting to server...\n");
    printf("Server address: %s\n", inet_ntoa(((struct sockaddr_in *)address)->sin_addr));
    printf("Server port: %d\n", ntohs(((struct sockaddr_in *)address)->sin_port));
    printf("Socket file descriptor: %d\n", sockfd);
    if (connect(sockfd, address, len) == -1) {
        perror("Connection error");
        exit(EXIT_FAILURE);
    }
}

void sendDataToServer(int sockfd, const char *data) {
    write(sockfd, data, strlen(data) + 1);
}

void receiveDataFromServer(int sockfd, char *buffer, size_t bufferSize) {
    int count = read(sockfd, buffer, bufferSize - 1);
    if (count == -1) {
        perror("Read error");
        exit(EXIT_FAILURE);
    }
    buffer[count] = '\0'; // Null-terminate the received data
}

int main() {
    int sockfd;
    struct sockaddr_in address;
    socklen_t len = sizeof(address);
    char instr[MAX_BUFFER_SIZE];
    char str[MAX_BUFFER_SIZE];

    sockfd = createSocket();
    setupServerAddress(&address);
    connectToServer(sockfd, (struct sockaddr *)&address, len);

    printf("\nEnter string: ");
    fgets(instr, MAX_BUFFER_SIZE, stdin);

    sendDataToServer(sockfd, instr);
    
    printf("Received data from server:\n");
    receiveDataFromServer(sockfd, str, sizeof(str));
    printf("%s\n", str);

    close(sockfd);
    exit(EXIT_SUCCESS);
}
