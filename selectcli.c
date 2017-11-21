#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#define MAXLINE 1024
#define IPADDRESS "0.0.0.0"
#define SERV_PORT 6666

#define max(a,b) (a > b) ? a : b

static void handle_connection(int sockfd)
{
    char sendline[MAXLINE],recvline[MAXLINE];
    int maxfdp,stdineof;
    fd_set readfds;
    fd_set writefds;
    int n;
    struct timeval tv;
    int retval = 0;

    while (1)
    {

        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        
        FD_SET(sockfd,&readfds);
        FD_SET(sockfd,&writefds);
        
        maxfdp = sockfd;

        tv.tv_sec = 1;
        tv.tv_usec = 0;

        retval = select(maxfdp+1,&readfds, &writefds,NULL,&tv);

        if (retval == -1) {
            return ;
        }

        if (retval == 0) {
            printf("client timeout.\n");
            continue;
        }

        if (FD_ISSET(sockfd, &readfds)) {
            n = read(sockfd,recvline,MAXLINE);
            if (n <= 0) {
                fprintf(stderr,"client: server is closed.\n");
                close(sockfd);
                FD_CLR(sockfd,&readfds);
                return;
            }
            
            printf("recv info is:[%s]\n", recvline);
        }
        
        if (FD_ISSET(sockfd, &writefds)) {
            printf("Plese input >:");
            fgets(sendline, sizeof(sendline), stdin);
            n = write(sockfd,sendline,MAXLINE);
            if (n <= 0) {
                fprintf(stderr,"client: server is closed.\n");
                close(sockfd);
                FD_CLR(sockfd,&readfds);
                return;
            }
            sleep(5);
        }

    }
}

int main(int argc,char *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET,SOCK_STREAM,0);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,IPADDRESS,&servaddr.sin_addr);

    int retval = 0;
    retval = connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
    if (retval < 0) {
        fprintf(stderr, "connect fail,error:%s\n", strerror(errno));
        return -1;
    }

    printf("client send to server .\n");
    write(sockfd, "", 32);

    handle_connection(sockfd);

    return 0;
}