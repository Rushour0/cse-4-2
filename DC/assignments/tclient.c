#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define MAX_BUFFER_SIZE 80
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9129

void backSpace(char *str)
{
    int len = strlen(str);
    for (int i = 0; i < len; i++)
        printf("\b");
    for (int i = 0; i < len; i++)
        printf(" ");
    for (int i = 0; i < len; i++)
        printf("\b");
}

int createSocket()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

void setupServerAddress(struct sockaddr_in *address)
{
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = inet_addr(SERVER_IP);
    address->sin_port = htons(SERVER_PORT);
}

void connectToServer(int sockfd, struct sockaddr *address, socklen_t len)
{
    printf("Connecting to server...\n");
    printf("Server address: %s\n", inet_ntoa(((struct sockaddr_in *)address)->sin_addr));
    printf("Server port: %d\n", ntohs(((struct sockaddr_in *)address)->sin_port));
    printf("Socket file descriptor: %d\n", sockfd);
    if (connect(sockfd, address, len) == -1)
    {
        perror("Connection error");
        exit(EXIT_FAILURE);
    }
}

void sendDataToClient(int sockfd, const char *data)
{
    write(sockfd, data, strlen(data) + 1);

    if (strcmp(data, "exit") == 0)
    {
        printf("Client requested to close connection\n");
        exit(EXIT_SUCCESS);
    }
}

void receiveDataFromServer(int sockfd, char *buffer, size_t bufferSize)
{
    int count = read(sockfd, buffer, bufferSize - 1);
    if (count == 0)
    {
        printf("Server closed connection\n");
        exit(EXIT_SUCCESS);
    }
    else if (count == -1)
    {
        perror("Read error");
        exit(EXIT_FAILURE);
    }
    buffer[count] = '\0'; // Null-terminate the received data
}

void *sendThread(void *arg)
{
    int sockfd = *((int *)arg);
    char instr[MAX_BUFFER_SIZE];

    while (1)
    {
        printf("Enter message to send: ");
        fgets(instr, MAX_BUFFER_SIZE, stdin);
        instr[strcspn(instr, "\n")] = '\0'; // Remove the newline character from the input
        sendDataToClient(sockfd, instr);
    }
}

void *recvThread(void *arg)
{
    int sockfd = *((int *)arg);
    char str[MAX_BUFFER_SIZE];

    while (1)
    {
        receiveDataFromServer(sockfd, str, sizeof(str));

        if (strcmp(str, "exit") == 0)
        {
            printf("Server closed connection\n");
            exit(EXIT_SUCCESS);
            close(sockfd);
        }
        backSpace("Enter message to send: ");
        printf("Server: %s\n", str);
        printf("Enter message to send: ");
        fflush(stdout);
    }
}

int main()
{
    int sockfd;
    struct sockaddr_in address;
    socklen_t len = sizeof(address);

    sockfd = createSocket();
    setupServerAddress(&address);
    connectToServer(sockfd, (struct sockaddr *)&address, len);

    pthread_t sendThreadId;
    pthread_create(&sendThreadId, NULL, sendThread, (void *)&sockfd);
    // pthread_create(&recvThreadId, NULL, recvThread, (void *)&sockfd);

    char str[MAX_BUFFER_SIZE];

    while (1)
    {
        receiveDataFromServer(sockfd, str, sizeof(str));

        if (strcmp(str, "exit") == 0)
        {
            printf("Server closed connection\n");
            exit(EXIT_SUCCESS);
            close(sockfd);
        }
        backSpace("Enter message to send: ");
        printf("Server: %s\n", str);
        printf("Enter message to send: ");
        fflush(stdout);
    }
    exit(EXIT_SUCCESS);
}
