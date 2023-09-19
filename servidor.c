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

    char logbuf[MAXDATASIZE + 1];
    snprintf(logbuf, sizeof(logbuf), "Bound to %d\n", (int)ntohs(servaddr.sin_port));
    Log(logbuf);

    if (listen(listenfd, LISTENQ) == -1) {
        perror("listen");
        exit(1);
    }

    pid_t connpid;

    for ( ; ; ) {
        connfd = Accept(listenfd, (struct sockaddr *) NULL, NULL);
        
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
            snprintf(logbuf, sizeof(logbuf), "Remote client connected: %s: %d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
            Log(logbuf);

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
                char s[30];
                if(rand() % 2)
                    strcpy(s, "SIMULE: MEMORIA_INTENSIVA\n");
                else
                    strcpy(s, "SIMULE: CPU_INTENSIVA\n");
                if(cnt == 0)
                    strcpy(s, "DC\n");

                snprintf(cbuf, sizeof(cbuf), "%s", s);
                write(connfd, cbuf, strlen(cbuf));
               
                snprintf(logbuf, sizeof(logbuf), "Sending -> %s", s);
                Log(logbuf);

                //Recebe dados do cliente. Recebe exatamente uma linha do stdin do cliente, então para quando recebe um '\n'
                int n;
                while((n = read(connfd, cbuf, MAXDATASIZE)) > 0)
                {
                    cbuf[n] = 0;
                    // if(fputs(cbuf, stdout) == EOF)
                    // {
                    //     perror("fputs");
                    //     exit(1);
                    // }
                    snprintf(logbuf, sizeof(logbuf) * 2, "Recieved -> %s", cbuf);
                    Log(logbuf);

                    if(cbuf[n - 1] == '\n')
                        break;
                }
            }

            close(connfd);
			exit(0);
        }        
    }
    return(0);
}
