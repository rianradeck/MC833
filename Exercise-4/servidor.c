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

	while(1)
	{
		fd_set readfds;//* = allfds;
		memcpy(&readfds, &allfds, sizeof(fd_set));
		select(maxfd + 1, &readfds, NULL, NULL, NULL);
		if(FD_ISSET(tcplistenfd, &readfds))
		{
			tcpclient = Accept(tcplistenfd, NULL, NULL);
			FD_SET(tcpclient, &allfds);
			maxfd = max(maxfd, tcpclient);
			
			struct sockaddr_in peeraddr;
			socklen_t peersz = sizeof(peeraddr);

			if(getpeername(tcpclient, (struct sockaddr*)&peeraddr, &peersz) == -1)
			{
				perror("getpeername");
				exit(1);
			}
			printf("TCP client connected %s: %d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));

			char buff[2048], time_str[1024];
			get_time(time_str);
			sprintf(buff, "Hello from server to client in:\nIP address: %s\nPort: %d\nTime:%s\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port), time_str);
			send(tcpclient, buff, strlen(buff) + 1, 0);
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
			printf("Recv from tcp %s %d\n", buff, read);
			send(tcpclient, buff, read + 1, 0);
		}
		if(FD_ISSET(udpboundfd, &readfds))
		{
			char buff[1024];
			struct sockaddr_in udp_addr;
			socklen_t addrlen = sizeof(udp_addr);

			int read = recvfrom(udpboundfd, buff, sizeof(buff) - 1, 0, (struct sockaddr*)&udp_addr, &addrlen);
			if(read < 0)
			{
				perror("recvfrom errror");
				exit(0);
			}
			buff[read] = 0;
			printf("Message from UDP client: %s\n", buff);

			printf("UDP client %s: %d\n", inet_ntoa(udp_addr.sin_addr), ntohs(udp_addr.sin_port));

			sprintf(buff, "Hello Client UDP\n");
			if(sendto(udpboundfd, buff, strlen(buff) + 1, 0, (struct sockaddr*)&udp_addr, addrlen) == -1)
			{
				perror("Sendto error");
				exit(0);
			}
			printf("Message %s sent to udp client\n", buff);
		}
	}
}
