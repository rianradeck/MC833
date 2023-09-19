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
    char   error[MAXLINE + 1];
    struct sockaddr_in servaddr;
    
    //Adicionamos mais um parametro para que o cliente possa
    //especificar qual porta remota ele deseja se conectar
    if (argc != 3) {
        strcpy(error,"uso: ");
        strcat(error,argv[0]);
        strcat(error," <IPaddress>");
        strcat(error," <Port>");
        perror(error);
        exit(1);
    }

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        perror("inet_pton error");
        exit(1);
    }

    Connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    /* Codigo para obter (#IP, #porta local) da questao 5*/
    socklen_t sz = sizeof(servaddr);
    Getsockname(sockfd, &servaddr);

    printf("Local: %s %d\n", inet_ntoa(servaddr.sin_addr), (int)ntohs(servaddr.sin_port));

    //socklen_t sz;
    bzero(&servaddr, sizeof(servaddr));
    if(getpeername(sockfd, (struct sockaddr*)&servaddr, &sz) == -1)
    {
        perror("getpeername");
        exit(1);
    }
    printf("Remote: %s %d\n", inet_ntoa(servaddr.sin_addr), (int)ntohs(servaddr.sin_port));

    //char buff[MAXLINE + 1];
    //fgets(buff, MAXLINE, stdin);
    //write(sockfd, buff, strlen(buff));

    for(;;){
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
        
        if(strcmp(recvline, "SIMULE: CPU_INTENSIVA\n") == 0)
        {
            puts("Starting CPU stress");
            sleep(5);
            char response[] = "SIMULACAO: CPU_INTENSIVA CONCLUIDA\n";
            write(sockfd, response, strlen(response));
        }
        else if(strcmp(recvline, "SIMULE: MEMORIA_INTENSIVA\n") == 0)
        {
            puts("Starting MEM stress");
            sleep(5);
            char response[] = "SIMULACAO: MEMORIA_INTENSIVA CONCLUIDA\n";
            write(sockfd, response, strlen(response));
        } else if(strcmp(recvline, "DC\n") == 0){
            char response[] = "DISCONNECTED\n";
            write(sockfd, response, strlen(response));
            break;
        } else {
            char response[] = "OPERACAO INVALIDA\n";
            write(sockfd, response, strlen(response));
        }
        recvline_offset = 0;
    }
    close(sockfd);
    
    exit(0);
}
