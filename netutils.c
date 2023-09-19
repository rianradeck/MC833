#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "netutils.h"

#define WRAPPER(X, ...) int ret = X(__VA_ARGS__); \
if(ret < 0) \
{ \
	perror(#X); \
} \
return ret

#define DEBUG 1

int Socket(int fam, int type, int flgs) { WRAPPER(socket, fam, type, flgs); }

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen){ WRAPPER(accept, sockfd, addr, addrlen); }

int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { WRAPPER(bind, sockfd, addr, addrlen); }

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { WRAPPER(connect, sockfd, addr, addrlen); }

void get_time(char buf[]){
	struct timeval tp;
	gettimeofday(&tp, 0);
	time_t curtime = tp.tv_sec;
	struct tm *t = localtime(&curtime);
	snprintf(buf, sizeof(char) * 58, "%02d:%02d:%02d.%03ld", t->tm_hour, t->tm_min, t->tm_sec, tp.tv_usec/1000);
}

void Log(char buf[]){
	int pid = getpid();
	char stime[4096];
	get_time(stime);
	printf("[%s] %d: %s\n", stime, pid, buf);
}