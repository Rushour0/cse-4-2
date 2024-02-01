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

void *server_thread(void *arg) {
    int client_sockfd = (int)arg;
    char instr[MAX_BUFFER_SIZE], name[MAX_BUFFER_SIZE];

    int count = read(client_sockfd, instr, MAX_BUFFER_SIZE - 1);
    instr[count] = '\0';
    printf("Input from client: %s\n", instr);

    strcpy(name, "Thank you");
    strcat(name, instr);
    strcpy(instr, name);

    write(client_sockfd, instr, strlen(instr) + 1);
    close(client_sockfd);

    pthread_exit(NULL);
}

int main() {
    int server_sockfd, client_sockfd;
    int server_len, client_len;
    struct sockaddr_in server_address, client_address;
    pthread_t t;

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(SERVER_PORT);

    server_len = sizeof(server_address);

    if (bind(server_sockfd, (struct sockaddr *)&server_address, server_len) == -1) {
        perror("Server error");
        exit(EXIT_FAILURE);
    }

    printf("Server waiting....\n");

    listen(server_sockfd, 5);

    while (1) {
        client_len = sizeof(client_address);
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);

        printf("Created new client thread %d\n", client_sockfd);
        pthread_create(&t, NULL, server_thread, (void *)(intptr_t)client_sockfd);
    }

    return 0;
}
