#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <poll.h>
#include	<time.h>

#define ADDR_IP "127.0.0.1"
#define PORT    4444
#define LISTENNUM 20
#define BUFSIZE 1024

int main(int argc, char *argv[])
{
    int sockfd, accfd;
    int seconds = 0;
    int iRet = 0;
    int len = 0;
    char buf[BUFSIZE]={0};
    
    
    struct sockaddr_in servaddr;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "create socket fail.[%s]", strerror(errno));
        return -1;
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &seconds, sizeof(seconds)) < 0)
    {
        fprintf(stderr, "setsockopt fail. [%s]", strerror(errno));
        return -1;
    }
    
    bzero(&servaddr, sizeof(servaddr));
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ADDR_IP);
    servaddr.sin_port = htons(PORT);
    
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        fprintf(stderr, "bind fail. [%s]", strerror(errno));
        return -1;
    }
    listen(sockfd, LISTENNUM);
    
        len = sizeof(servaddr);
        /*accfd is accept function new back description*/
        accfd = accept(sockfd, (struct sockaddr *)&servaddr, &len);
        if (accfd < 0)
        {
            fprintf(stderr, "accept fail. [%s]", strerror(errno));
            return -1;
        }
            
        write(accfd, "Hello world", 32);    
            
    return 0;
}