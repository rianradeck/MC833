#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "netutils.h"

#define MAXLINE 4096

int main(int argc, char **argv) {
    int    sockfd, n;
    int    recvline_offset = 0;
    char   recvline[MAXLINE + 1];
    char   buf[MAXLINE + 1];
    char   error[MAXLINE + 1];
    struct sockaddr_in servaddr;
    
    if (argc != 3) {
        strcpy(error,"uso: ");
        strcat(error,argv[0]);
        strcat(error," <IPaddress>");
        strcat(error," <Port>");
        perror(error);
        exit(1);
    }

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr = inet_addr(argv[1]); 

    Connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    strcpy(buf, "Hello, I am an UDP client.");

    if (sendto(sockfd, buf, strlen(buf) + 1, 0, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("read error");
        exit(2);
    }

    while ( (n = read(sockfd, recvline + recvline_offset, MAXLINE - recvline_offset)) > 0) {
        recvline_offset += n;
        if(recvline[recvline_offset - 1] == '\n')
            break;
    }

    if (n < 0) {
        perror("read error");
        exit(1);
    }

    recvline[recvline_offset] = 0;

    puts("-------- RECIEVED FROM SERVER -----------");
    if (fputs(recvline, stdout) == EOF) {
        perror("fputs error");
        exit(1);
    }
    puts("-----------------------------------------");
    
    close(sockfd);
    
    exit(0);
}