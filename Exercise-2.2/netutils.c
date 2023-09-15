#include <stdlib.h>
#include <stdio.h>

#include "netutils.h"

#define WRAPPER(X, ...) int ret = X(__VA_ARGS__); \
if(ret < 0) \
{ \
	perror(#X); \
} \
return ret


int Socket(int fam, int type, int flgs) { WRAPPER(socket, fam, type, flgs); }

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen){ WRAPPER(accept, sockfd, addr, addrlen); }

int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { WRAPPER(bind, sockfd, addr, addrlen); }

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { WRAPPER(connect, sockfd, addr, addrlen); }
