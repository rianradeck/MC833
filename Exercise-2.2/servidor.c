#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define LISTENQ 10
#define MAXDATASIZE 100

int main (int argc, char **argv) {
    int    listenfd, connfd;
    struct sockaddr_in servaddr, peeraddr;
    char   buf[MAXDATASIZE + 1];
    time_t ticks;


    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //Escolhemos porta 0 para que o SO atribua automaticamente uma porta
    //disponivel para o socket (Questao 4)
    servaddr.sin_port        = htons(0);   
    
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind");
        exit(1);
    }

    //Nesse trecho do codigo computamos e imprimimos a porta em que o servidor está bound e que irá fazer listening, para que o cliente possa se conectar a essa porta
    socklen_t sz = sizeof(servaddr);
    if(getsockname(listenfd, (struct sockaddr*)&servaddr, &sz) == -1)
    {
        perror("getsockname");
        exit(1);
    }

    printf("Bound to %d\n", (int)ntohs(servaddr.sin_port));

    if (listen(listenfd, LISTENQ) == -1) {
        perror("listen");
        exit(1);
    }

    pid_t connpid;

    for ( ; ; ) {
        if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1)
        {
            perror("accept");
            exit(1);
        }
        if((connpid = fork()) == 0)
        {
            close(listenfd);

            //Computa e imprime o ip e a porta do cliente: (Questao 6)
            socklen_t peersz = sizeof(peeraddr);
            if(getpeername(connfd, (struct sockaddr*)&peeraddr, &peersz) == -1)
            {
                perror("getpeername");
                exit(1);
            }
            printf("Remote client connected: %s: %d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
        

            ticks = time(NULL);
            snprintf(buf, sizeof(buf), "Hello from server!\nTime: %.24s\r\n", ctime(&ticks));
            write(connfd, buf, strlen(buf));

            //Recebe dados do cliente. Recebe exatamente uma linha do stdin do cliente, então para quando recebe um '\n'
            int n;
            while((n = read(connfd, buf, MAXDATASIZE)) > 0)
            {
                buf[n] = 0;
                if(fputs(buf, stdout) == EOF)
                {
                    perror("fputs");
                    exit(1);
                }
                // if(buf[n - 1] == '\n')
                //     break;
                if(strcmp(buf, "exit") == 0)
                    break;
            }
            
            close(connfd);
			exit(0);
        }        
    }
    return(0);
}
