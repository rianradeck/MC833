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

#include "netutils.h"

#define MAXDATASIZE 4096

int main (int argc, char **argv) {
    int    listenfd, connfd;
    struct sockaddr_in servaddr, peeraddr;
    char   buf[MAXDATASIZE + 1];
    char   error[MAXDATASIZE + 1];
    time_t ticks;
    int port = 0;
    int backlog = 0;

    if (argc != 3) {
    strcpy(error,"uso: ");
        strcat(error,argv[0]);
        strcat(error," <Port>");
        strcat(error," <Backlog>");
        perror(error);
        exit(1);
    }
    FILE *fp;
    fp = fopen("log.txt", "w");
    fclose(fp);

    port = atoi(argv[1]);
    backlog = atoi(argv[2]);

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(port);   
    
    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    Getsockname(listenfd, &servaddr);

    char logbuf[MAXDATASIZE + 31];
    snprintf(logbuf, sizeof(logbuf), "Bound to %d\n", (int)ntohs(servaddr.sin_port));
    Log(logbuf);

    Listen(listenfd, backlog);

    pid_t connpid;

    int cnt = 0;
    printf("Backlog: %d\n", backlog);
    for ( ; ; ) {
        sleep(1); // Adicione um sleep() na primeira linha do laço no servidor para poderem visualizar a mudança do backlog.
        // continue;
        connfd = Accept(listenfd, (struct sockaddr *) NULL, NULL);
        printf("CONEXAO %d - %d\n", ++cnt, connfd);
        fflush(stdout);
        if((connpid = fork()) == 0)
        {
            close(listenfd);

            socklen_t peersz = sizeof(peeraddr);
            if(getpeername(connfd, (struct sockaddr*)&peeraddr, &peersz) == -1)
            {
                perror("getpeername");
                exit(1);
            }
            snprintf(logbuf, sizeof(logbuf), "Remote client connected: %s: %d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port)); 
            Log(logbuf); // Imprima no servidor IP e porta dos clientes conectados nele para identificar corretamente as conexões.

            ticks = time(NULL);
            snprintf(buf, sizeof(buf), "Hello from server!\nTime: %.24s\r\nPID: %d\n", ctime(&ticks), getpid());
            write(connfd, buf, strlen(buf));
            int k;
            while((k = read(connfd, buf, MAXDATASIZE)) > 0)
                if(buf[k - 1] == '\n')
                    break;
            Log("SENT A HELLO AND RECIEVED ANY RESPONSE, STARTING COMMUNICATION");

            // Comunication loop
            for(int cnt = 5;cnt--;)
            {
                char cbuf[MAXDATASIZE + 1];
                srand(time(NULL));
                char cmd[30];
                if(rand() % 2)
                    strcpy(cmd, "SIMULE: MEMORIA_INTENSIVA\n");
                else
                    strcpy(cmd, "SIMULE: CPU_INTENSIVA\n");
                if(cnt == 0)
                    strcpy(cmd, "DC\n");

                snprintf(cbuf, sizeof(cbuf), "%s", cmd);
                write(connfd, cbuf, strlen(cbuf));
               
                snprintf(logbuf, sizeof(logbuf), "Sending -> %s", cmd);
                Log(logbuf);

                //Recebe dados do cliente. Recebe exatamente uma linha do stdin do cliente, então para quando recebe um '\n'
                int n;
                while((n = read(connfd, cbuf, MAXDATASIZE)) > 0)
                {
                    cbuf[n] = 0;
                   
                    snprintf(logbuf, sizeof(logbuf), "Recieved -> %s", cbuf);
                    Log(logbuf);

                    if(cbuf[n - 1] == '\n')
                        break;
                }
            }

            close(connfd);
            exit(0);
        }
    }
    fclose(fp);
    return(0);
}
