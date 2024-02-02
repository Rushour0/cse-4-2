#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
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
    for (int i = 0; i < len; i++) printf("\b");
    for (int i = 0; i < len; i++) printf(" ");
    for (int i = 0; i < len; i++) printf("\b");

}

// Function to handle client communication in a separate thread
void *handleClient(void *arg)
{
    int client_sockfd = *((int *)arg);
    char client_input[MAX_BUFFER_SIZE];

    while (1)
    {
        // Receive data from the client
        ssize_t bytes_received = read(client_sockfd, client_input, sizeof(client_input) - 1);
        if (bytes_received == -1)
        {
            perror("Error reading from client");
            break;
        }
        else if (bytes_received == 0)
        {
            perror("Client disconnected\n");
            break;
        }

        client_input[bytes_received] = '\0';

        if (strcmp(client_input, "exit") == 0)
        {
            printf("Client requested to close connection\n");
            break;
        }

        backSpace("Enter message to send: ");
        printf("Client: %s\n", client_input);
        printf("Enter message to send: ");
        fflush(stdout);
    }

    close(client_sockfd);
    exit(EXIT_SUCCESS);
}

// Function to set up the server socket
int setupServerSocket()
{
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd == -1)
    {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(SERVER_PORT);

    if (bind(server_sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Error binding server socket");
        close(server_sockfd);
        exit(EXIT_FAILURE);
    }

    return server_sockfd;
}

void sendDataToClient(int sockfd, const char *data)
{
    write(sockfd, data, strlen(data) + 1);
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

int main()
{
    int server_sockfd = setupServerSocket();
    listen(server_sockfd, 5);

    while (1)
    {
        struct sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);
        int client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);

        if (client_sockfd == -1)
        {
            perror("Error accepting client connection");
            continue;
        }

        printf("Accepted new client connection on socket %d\n", client_sockfd);

        // Create a thread to handle the client communication
        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handleClient, (void *)&client_sockfd) != 0)
        {
            perror("Error creating client thread");
            close(client_sockfd);
        }
        else
        {
            pthread_detach(client_thread); // Detach the thread to allow it to clean up after completion
        }
        pthread_t send_thread;
        if (pthread_create(&send_thread, NULL, sendThread, (void *)&client_sockfd) != 0)
        {
            perror("Error creating send thread");
            close(client_sockfd);
        }
        else
        {
            pthread_detach(send_thread); // Detach the thread to allow it to clean up after completion
        }
    }

    close(server_sockfd);
    exit(EXIT_SUCCESS);
}
