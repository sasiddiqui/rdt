// Sayeed Siddiqui - CSE 3461 miniLab
// Usage: ./receiver hostname port filename

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>

void error (char *msg) {
    perror(msg);
    exit(1);
}

int main (int argc, char *argv[]) {
    int sockfd;
    int port, n;
    struct sockaddr_in server_addr;
    struct hostent *server;
    
    char buffer[256];
    if (argc < 3) error("Usage: receiver hostname port filename\n");
    
    // create socket
    port = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd < 0) error("Could not open socket\n");
    
    // get server info
    server = gethostbyname(argv[1]);
    if (server == NULL) error("No such host\n");
    
    // initialize server struct
    bzero((char*) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char*)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(port);
    
    // establish connection
    printf("Attempting to connect..\n");
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        error("Could not connect\n");
    
    while (1) {
        // prompt user for a message
        printf("Enter a file to request: ");
        bzero(buffer, 256);
        fgets(buffer, 256, stdin);
    
        // send to server
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) error("Could not write to socket\n");
    
        // read response
        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);
        if (n < 0) error("Could not read from socket\n");
        printf("Received: %s\n", buffer);
    }

    // close socket
    close(sockfd);
    
    return 0;
}
