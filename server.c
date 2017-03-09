// Sayeed Siddiqui - CSE 3461 Lab 1
// Usage: ./server port

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#define BUFFER 1000
void error (char* msg) {
    perror(msg);
    exit(1);
}

int main (int argc, char* argv[]) {
    int sockfd, newsockfd;
    int port;
    int n, ext, i, len;
    socklen_t client_len;
    char buffer[BUFFER+1];
    char* type;
    FILE* fd;

    struct sockaddr_in server_addr, client_addr;
    if (argc < 2) error("Usage: server port\n");

    // create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("Could not open socket\n");

    bzero((char*)&server_addr, sizeof(server_addr));
    port = atoi(argv[1]);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        error("Could not bind\n");

    listen(sockfd, 5); 
    client_len = sizeof(client_addr);

    // this loop perpetually accepts the connection and processes a new HTTP request
    while (1) {

        newsockfd = accept(sockfd, (struct sockaddr*) &client_addr, &client_len);
        if (newsockfd < 0) error("Could not accept\n");

        bzero(buffer, BUFFER);
        n = read(newsockfd, buffer, BUFFER);

        // terminate buffer string
        if (n > 0 && n < BUFFER) {
            buffer[n] = 0;
        } else {
            buffer[0] = 0;
        }

        printf("Received request: \n%s", buffer);

        // open file for reading
        if (access(&buffer[5], F_OK) == -1) {
            flag404 = 1;
            fd = fopen("404.html", "r");
        } else {
            fd = fopen(&buffer[5], "r");
        }
       
        // determine file length
        fseek(fd, 0L, SEEK_END);
        len = ftell(fd);
        fseek(fd, 0L, SEEK_SET);
        printf("Sending file of length %d\n", len);

        bzero(buffer, BUFFER);

        write(newsockfd, buffer, strlen(buffer));
        printf("Responded: %s\n", buffer);

        // send back specified file in BUFFER-sized chunks
        while (!feof(fd)) {
            n = fread(buffer, 1, BUFFER, fd);
            write(newsockfd, buffer, n);
            printf("sent %d bytes\n", n);
        }
        
        fclose(fd);
        printf("Sent\n");

        if (n < 0) error("Could not write to socket\n");

        sleep(1);
        close(newsockfd);
    }

    close(sockfd);
    return 0;
}
