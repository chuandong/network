#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>

#define IPADDR      "127.0.0.1"
#define PORT        8787
#define MAXLINE     1024
#define LISTENQ     5
#define SIZE        3

typedef struct server_context_st
{
    int cli_cnt;        /*client bumbers*/
    int clifds[SIZE];   /*client description*/
    fd_set allfds;      /*handle set*/
    int maxfd;          /*handle max value*/
} server_context_st;
static server_context_st *s_srv_ctx = NULL;

char filename[20]={0};

static int create_server_proc(const char* ip,int port)
{
    int  fd;
    struct sockaddr_in servaddr;
    fd = socket(AF_INET, SOCK_STREAM,0);
    if (fd == -1) {
        fprintf(stderr, "create socket fail,erron:%d,reason:%s\n",
                errno, strerror(errno));
        return -1;
    }

    /*A port freed waiting for two seconds later by use, SO_REUSEADDR effect is freed port can by used*/
    int reuse = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        return -1;
    }

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    /*servaddr.sin_addr = inet_addr(ip);*/
    inet_pton(AF_INET,ip,&servaddr.sin_addr);
    servaddr.sin_port = htons(port);

    if (bind(fd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == -1) {
        perror("bind error: ");
        return -1;
    }

    listen(fd,LISTENQ);

    return fd;
}

static int accept_client_proc(int srvfd)
{
    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen;
    cliaddrlen = sizeof(cliaddr);
    int clifd = -1;
    static int i = 0;

    printf("%05d accpet clint proc is called.\n", __LINE__);

    while (1)
    {
        clifd = accept(srvfd,(struct sockaddr*)&cliaddr,&cliaddrlen);

        if (clifd == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                fprintf(stderr, "%05d accept fail,error:%s\n", __LINE__, strerror(errno));
                return -1;
            }
        }
        
        fprintf(stdout, "%05d accept a new client: %s:%d\n", __LINE__,
            inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);
    

        /*add new connect descriptor to array*/
        for (i = 0; i < SIZE; i++) {
            if (s_srv_ctx->clifds[i] < 0) {
                s_srv_ctx->clifds[i] = clifd;
                s_srv_ctx->cli_cnt++;
                break;
            }
        }
        
        printf("%05d i:[%d]\n", __LINE__, i);
        if (i == SIZE) {
            fprintf(stderr,"%05d too many clients.\n", __LINE__);
            return -1;
        }
        
        break;
    }
    
    return 0;
}

/*static int handle_client_msg(int fd, char *buf) 
{
    if (strlen(buf) == 0)
    sprintf(buf, "Please input infomation give service.");    
    write(fd, buf, strlen(buf) +1);
    return 0;
}*/

static void recv_client_msg(fd_set *readfds)
{
    int i = 0, n = 0;
    int clifd;
    char buf[MAXLINE] = {0};
    for (i = 0;i <= s_srv_ctx->cli_cnt;i++) {
        clifd = s_srv_ctx->clifds[i];
        if (clifd < 0) {
            continue;
        }
        
        /*judge client socket if have data*/
        if (FD_ISSET(clifd, readfds)) {
            /*recv client send infomation*/
            n = read(clifd, buf, MAXLINE);
            if (n <= 0) {
                /*n==0 express read accomplish, client close socket*/
                FD_CLR(clifd, &s_srv_ctx->allfds);
                close(clifd);
                s_srv_ctx->clifds[i] = -1;
                continue;
            }
            sleep(5);
            printf("recv buf is :[%s]\n", buf);
            /*handle_client_msg(clifd, buf);*/
            
        }
    }
}

static void handle_client_proc(int srvfd)
{
    int  clifd = -1;
    int  retval = 0;
    struct timeval tv;
    int i = 0;
    char writebuf[MAXLINE]={0};
    
    fd_set *readfds = &s_srv_ctx->allfds;
    fd_set *writefds= &s_srv_ctx->allfds;

    while (1) {
        /*everytime transfer select needs to set file description and time, because the file description and time could modify by kernel when the function finished*/
        FD_ZERO(readfds);
        FD_ZERO(writefds);
        /*add listen socket*/
        FD_SET(srvfd, readfds);
        FD_SET(srvfd, writefds);
        s_srv_ctx->maxfd = srvfd;

        tv.tv_sec = 30;
        tv.tv_usec = 0;
        printf("%s %05d connect client numbers is:[%d]\n", filename, __LINE__, s_srv_ctx->cli_cnt);
        /*add client socket*/
        for (i = 0; i < s_srv_ctx->cli_cnt; i++) {
            clifd = s_srv_ctx->clifds[i];
            /*drop useless handles*/
            if (clifd != -1) {
                FD_SET(clifd, readfds);
            }
            s_srv_ctx->maxfd = (clifd > s_srv_ctx->maxfd ? clifd : s_srv_ctx->maxfd);
            printf("%5d maxfd:[%d]", __LINE__, s_srv_ctx->maxfd);
        }

        /*start select recv deal with service and client socket*/
        retval = select(s_srv_ctx->maxfd + 1, readfds, writefds, NULL, &tv);
        if (retval == -1) {
            fprintf(stderr, "select error:%s.\n", strerror(errno));
            return;
        }
        if (retval == 0) {
            fprintf(stdout, "select is timeout.\n");
            continue;
        }
        
        printf("%d FD_ISSET:[%d]", __LINE__, FD_ISSET(srvfd, readfds));
        if (FD_ISSET(srvfd, readfds)) {
            /*listen client request*/
            accept_client_proc(srvfd);
        } else {
            
            printf("%d esle FD_ISSET:[%d]", __LINE__, FD_ISSET(srvfd, readfds));
            /*recv client message*/
            recv_client_msg(readfds);
        }
        /*if (FD_ISSET(srvfd, writefds))
        {
            printf("writefds need you input >:\n");
            fgets(writebuf, sizeof(writebuf), stdin);
            write(srvfd, writebuf, strlen(writebuf));
        }*/
        
        if (s_srv_ctx->cli_cnt == 0)
        {
            printf("try to connection...");
            sleep(5);
            continue;
        }
    }
}

static void server_uninit()
{
    if (s_srv_ctx) {
        free(s_srv_ctx);
        s_srv_ctx = NULL;
    }
}

static int server_init()
{
    s_srv_ctx = (server_context_st *)malloc(sizeof(server_context_st));
    if (s_srv_ctx == NULL) {
        return -1;
    }

    memset(s_srv_ctx, 0, sizeof(server_context_st));

    int i = 0;
    for (;i < SIZE; i++) {
        s_srv_ctx->clifds[i] = -1;
    }

    return 0;
}

int main(int argc,char *argv[])
{
    int  srvfd;
    
    memcpy(filename, argv[0], strlen(argv[0]));
    
    /*init service context*/
    if (server_init() < 0) {
        return -1;
    }
    /*create service bind and linsten*/
    srvfd = create_server_proc(IPADDR, PORT);
    if (srvfd < 0) {
        fprintf(stderr, "socket create or bind fail.\n");
        server_uninit();
        return -1;
    }
   
    /*start recv and deal with client request*/
    handle_client_proc(srvfd);
    server_uninit();
    return 0;
}