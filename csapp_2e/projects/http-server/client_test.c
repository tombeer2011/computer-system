
#include "csapp.h"
#define N 100
struct Argv 
{
    char* host;
    int port;
};

void* thread(void* vargp) 
{
    Pthread_detach(pthread_self());
    struct Argv* args = (struct Argv*)(vargp);
    char* host = args->host;
    char buf[MAXLINE];
    rio_t rio;
    int port = args->port;
    int clientfd;
    while ((clientfd = Open_clientfd(host, port)) < 0) {}

    Rio_readinitb(&rio, clientfd);
    const char* contents = "GET / HTTP1.1\n";

    strcpy(buf, contents);
    Rio_writen(clientfd, buf, strlen(buf));
    int n;
    while (1) {
        int n = Rio_readlineb(&rio, buf, MAXLINE);
        if (n != 0) break;
        Fputs(buf, stdout);
    }
   // 如果一方关闭了fd，那么另一方的read或者write应该返回-1.
    Close(clientfd);
    free(host);
    free(args);
    return NULL;
}

int main(int argc, char **argv)
{
    int i;
    if (argc != 3) {
    	   fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
    	   exit(0);
    }
    // host = argv[1];
    // port = atoi(argv[2]);
    pthread_t tid;
    for (i=0; i<N; i++) {
        printf("%d is created\n", i);
        struct Argv* temp = malloc(sizeof(struct Argv));
        temp->host = malloc(sizeof(argv[1]) + 1);
        temp->port = atoi(argv[2]);
        strcpy(temp->host, argv[1]);
        Pthread_create(&tid, NULL, thread, (void*)temp);

    }
    Pthread_exit(NULL);

    exit(0);
}


