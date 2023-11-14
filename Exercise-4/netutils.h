#include <sys/socket.h>
#include <time.h>

int Socket(int fam, int type, int flgs);

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

int Listen(int sockfd, int backlog);

void Log(const char buf[]);

int Getsockname(int sockfd, struct sockaddr_in *addr);

void get_time(char buf[]);
