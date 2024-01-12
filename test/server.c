#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char const *argv[]) {

    int serverFd, clientFd;
    struct sockaddr_in server, client;
    unsigned int len;
    int port = 1234;
    char buffer[1024];
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        perror("Cannot create socket\n");
        exit(1);
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    len = sizeof(server);
    if (bind(serverFd, (struct sockaddr *)&server, len) < 0) {
        perror("Cannot bind socket\n");
        exit(2);
    }
    if (listen(serverFd, 10) < 0) {
        perror("Listen error\n");
        exit(3);
    }
    while (1) {
        len = sizeof(client);
        printf("Waiting for clients\n");
        if ((clientFd = accept(serverFd, (struct sockaddr *)&client, &len)) < 0) {
        perror("Accept error\n");
        exit(4);
        }
        char *client_ip = inet_ntoa(client.sin_addr);
        printf("Accepted new connection from a client %s:%d\n", client_ip, ntohs(client.sin_port));
        memset(buffer, 0, sizeof(buffer));
        int size = read(clientFd, buffer, sizeof(buffer));
        if ( size < 0 ) {
        perror("Read error\n");
        exit(5);
        }
        printf("Received\n %s \nfrom client\n", buffer);
    
    int i = 0;
    for (; i < size; i++)
        buffer[i] = toupper(buffer[i]);

        if (write(clientFd, buffer, size) < 0) {
        perror("Write error\n");
        exit(6);
        }
        close(clientFd);
    }
    close(serverFd);
    return 0;
}