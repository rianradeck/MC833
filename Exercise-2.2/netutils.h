#include <sys/socket.h>

int Socket(int fam, int type, int flgs);

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);