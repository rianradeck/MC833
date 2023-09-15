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

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        perror("inet_pton error");
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("connect error");
        exit(1);
    }

	/* Codigo para obter (#IP, #porta local) da questao 5*/
    bzero(&servaddr, sizeof(servaddr));
	socklen_t sz = sizeof(servaddr);
	if(getsockname(sockfd, (struct sockaddr*)&servaddr, &sz) == -1)
	{
		perror("getsockname");
		exit(1);
	}

	printf("Local: %s %d\n", inet_ntoa(servaddr.sin_addr), (int)ntohs(servaddr.sin_port));

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

	if (fputs(recvline, stdout) == EOF) {
		perror("fputs error");
		exit(1);
	}
	
	if(strcmp(recvline, "SIMULE: CPU_INTENSIVA") == 0)
	{
		sleep(1);
		char response[] = "SIMULACAO: CPU_INTENSIVA CONCLUIDA\n";
		write(sockfd, response, strlen(response));
	}
	else if(strcmp(recvline, "SIMULE: MEMORIA_INTENSIVA") == 0)
	{
		sleep(1);
		char response[] = "SIMULACAO: MEMORIA_INTENSIVA ONCLUIDA\n";
		write(sockfd, response, strlen(response));
	}

	close(sockfd);
	
    exit(0);
}
