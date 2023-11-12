#include <stdio.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include "netutils.h"

#define TCP_PORT 25565
#define UDP_PORT 1234
#define LISTENQ 10

#define max(a, b) ((a) > (b) ? (a) : (b))

int main()
{
	int tcpclient = -1;

	int tcplistenfd = Socket(AF_INET, SOCK_STREAM, 0);
	int udpboundfd = Socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in tcpaddr, udpaddr;
	bzero(&tcpaddr, sizeof(tcpaddr));
	tcpaddr.sin_family = AF_INET;
	tcpaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	tcpaddr.sin_port = htons(TCP_PORT);
	
	bzero(&udpaddr, sizeof(udpaddr));
	udpaddr.sin_family = AF_INET;
	udpaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	udpaddr.sin_port = htons(UDP_PORT);

	Bind(udpboundfd, (struct sockaddr *)&udpaddr, sizeof(udpaddr));
	Bind(tcplistenfd, (struct sockaddr *)&tcpaddr, sizeof(tcpaddr));

	Listen(tcplistenfd, LISTENQ);


	fd_set allfds;
	FD_ZERO(&allfds);
	FD_SET(tcplistenfd, &allfds);
	FD_SET(udpboundfd, &allfds);
	
	int maxfd = max(tcplistenfd, udpboundfd);
	int clientConnected = 0;

	while(1)
	{
		fd_set readfds = allfds;
		int nready = select(maxfd + 1, &readfds, NULL, NULL, NULL);
		if(FD_ISSET(tcplistenfd, &readfds))
		{
			tcpclient = Accept(tcplistenfd, NULL, NULL);
			FD_SET(tcpclient, &allfds);
			maxfd = max(maxfd, tcpclient);
			clientConnected = 1;
		}
		if(tcpclient != -1 && FD_ISSET(tcpclient, &readfds))
		{
			char buff[1024];
			int read = recv(tcpclient, buff, sizeof(buff) - 1, 0);
			if(read < 0)
			{
				printf("recv error\n");
				exit(0);
			}
			buff[read] = 0;
			printf("Recv from tcp %s\n", buff);
		}
		if(FD_ISSET(udpboundfd, &readfds))
		{
			char buff[1024];
			int read = recvfrom(udpboundfd, buff, sizeof(buff) - 1, 0, NULL, NULL);
			if(read < 0)
			{
				printf("recvfrom errror\n");
				exit(0);
			}
			buff[read] = 0;
			printf("Recv from udp %s\n", buff);
		}
	}
}
