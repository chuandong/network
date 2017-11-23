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

#define ADDR_IP "127.0.0.1"
#define PORT    4444
#define BUFSIZE 1024

int main(int argc, char *argv[])
{
    int sockfd;
    int iRet;
    char buf[BUFSIZE+1]={0};
    char buff[BUFSIZE+1]={0};
    
    struct sockaddr_in cliaddr;
    time_t tv;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "create socket fail.[%s]", strerror(errno));
        return -1;
    }
    
    bzero(&cliaddr, sizeof(cliaddr));
    
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = inet_addr(ADDR_IP);
    cliaddr.sin_port = htons(PORT);
    
    iRet = connect(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
    if (iRet < 0)
    {
        fprintf(stderr, "connect fail. [%s]", strerror(errno));
        return -1;
    }
    tv = time(NULL);
    
    while ((iRet=read(sockfd, buf, BUFSIZE)) > 0)
    {
        fputs(buf, stdout);
    }
    
    return 0;
}