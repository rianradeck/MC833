#include <stdio.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <string.h>
#include "netutils.h"

#define TCP_PORT 25565
#define LISTENQ 10

#define max(a, b) ((a) > (b) ? (a) : (b))

int main()
{
	int client[FD_SETSIZE];
	for(int i = 0; i < FD_SETSIZE; ++i)
		client[i] = -1;

	int tcplistenfd = Socket(AF_INET, SOCK_STREAM, 0);
	int udpboundfd = Socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in tcpaddr;
	bzero(&tcpaddr, sizeof(tcpaddr));
	tcpaddr.sin_family = AF_INET;
	tcpaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	tcpaddr.sin_port = htons(TCP_PORT);
	
	Bind(tcplistenfd, (struct sockaddr *)&tcpaddr, sizeof(tcpaddr));

	Listen(tcplistenfd, LISTENQ);


	fd_set allfds;
	FD_ZERO(&allfds);
	FD_SET(tcplistenfd, &allfds);
	
	int maxfd = tcplistenfd;
	int nfds = 0;

	while(1)
	{
		fd_set readfds;
		int nready = select(maxfd + 1, &readfds, NULL, NULL, NULL);
		if(FD_ISSET(tcplistenfd, &readfds))
		{
			int connfd = Accept(tcplistenfd, NULL, NULL);

			{
				int i;
				for(i = 0; i < FD_SETSIZE; ++i)
					if(client[i] < 0)
					{
						client[i] = connfd;
						break;
					}
				if(i == FD_SETSIZE)
				{
					printf("TOO MANY CLIENTS");
					return 0;
				}
				nfds = max(nfds, i);
			}
			
			maxfd = max(maxfd, connfd);
			if(--nready <= 0)
				continue;
		}

		for(int i = 0; i < nfds; ++i)
		{
			int curClient = client[i];
			if(curClient < 0)
				continue;
			if(FD_ISSET(curClient, &readfds))
			{

			}

			if(--nready <= 0)
				break;
		}

	}
}
