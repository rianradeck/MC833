#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include "netutils.h"

#define MAXLINE 4096
#define max(a, b) ((a) > (b) ? (a) : (b))

int main(int argc, char **argv)
{
	char sendline[MAXLINE + 1], recvline[MAXLINE + 1];
	int sockfd;
	char error[MAXLINE + 1];
	struct sockaddr_in servaddr;
	if (argc != 3)
	{
		strcpy(error, "uso: ");
		strcat(error, argv[0]);
		strcat(error, " <IPaddress>");
		strcat(error, " <Port>");
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
	
	fd_set allfds;
	FD_ZERO(&allfds);

	FILE *finput = stdin;

	FD_SET(fileno(finput), &allfds);
	FD_SET(sockfd, &allfds);

	int maxfd = max(fileno(finput), sockfd);

	recvline[MAXLINE] = 0;
	while (1)
	{
		fd_set readfds;// = allfds;
		memcpy(&readfds, &allfds, sizeof(fd_set));
		select(maxfd + 1, &readfds, NULL, NULL, NULL);
		if(FD_ISSET(sockfd, &readfds))
		{
			int read;
			if((read = recv(sockfd, recvline, MAXLINE, 0)) < 0)
			{
				fprintf(stderr, "read error");
				exit(1);
			}
			if(read == 0)
			{
				printf("EOF reached\n");
				break;
			}
			recvline[read] = 0;
			//printf("Received: ");
			fputs(recvline, stdout);
			fflush(stdout);
		}
		if(FD_ISSET(fileno(finput), &readfds))
		{
			int shouldsdwn = 0;
			if(fgets(sendline, MAXLINE, finput) == NULL)
			{
				//EOF Found, issuing shutdown()
				shouldsdwn = 1;

				//fputs(sendline, stdout);
				//exit(1);
			}
			if(send(sockfd, sendline, strlen(sendline), 0) < 0)
			{
				printf("Write error!\n");
			}
			if(shouldsdwn)
			{
				printf("Shutting down\n");	
				shutdown(sockfd, SHUT_WR);
			}
		}
	}
}
