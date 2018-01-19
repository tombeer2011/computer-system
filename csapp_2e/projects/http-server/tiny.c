/*
 * tiny.c - A simple, iterative HTTP/1.0 Web server that uses the
 *     GET method to serve static and dynamic content.
 *     编译 gcc -tiny.c csapp.c -lpthread
 */

#include "csapp.h"
#include <syslog.h>

typedef struct {                /* Represents a pool of connected descriptors */

        int maxfd;              /* Largest descriptor in read_set */
        fd_set read_set;        /* Set of all active descriptors */

        fd_set ready_set;       /* Subset of descriptors ready for reading */
        int nready;             /* Number of ready descriptors from select */

        int maxi;               /* Highwater index into client array */
        int clientfd[FD_SETSIZE]; /* Set of active descriptors */
        rio_t clientrio[FD_SETSIZE]; /* Set of active read buffers */
} pool;


void init_pool(int listenfd, pool *p);
void add_client(int connfd, pool *p);
void check_client(pool *p);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);


/* child process call this function */
void client_offline(int signo, siginfo_t *info, void *context)
{
	    printf("client offline founded...\n");
        /* If client is no longer alive, we just log something useful and exit */
        openlog("tiny", LOG_CONS, LOG_WARNING);
        syslog(LOG_WARNING,
               "pid %d: caught signal %d, exit with status code -1\n",
               info->si_pid,
               info->si_signo);
        closelog();
        exit(-1);
}

int main(int argc, char **argv)
{
        int listenfd, connfd, port, ret;
        socklen_t clientlen = sizeof(struct sockaddr_in);
        struct sockaddr_in clientaddr;
        static pool pool;

        if (argc != 2) {
                fprintf(stderr, "usage: %s <port>\n", argv[0]);
                exit(0);
        }
        port = atoi(argv[1]);

	    /* Install SIGPIPE handler */
	    struct sigaction action, old_action;
        memset(&action, 0, sizeof(action));
        action.sa_sigaction = client_offline;
        sigemptyset(&action.sa_mask);
        action.sa_flags = SA_SIGINFO;
        ret = sigaction(SIGPIPE, &action, &old_action);
        if (ret < 0) {
                fprintf(stderr,
                        "sigaction: failed to install SIGPIPE handler, %s",
                        strerror(errno));
        }
	

        listenfd = Open_listenfd(port);
        init_pool(listenfd, &pool);

	

        while (1) {
                /* Wait for listening/connected descriptor(s) to become ready */
                pool.ready_set = pool.read_set;
                pool.nready = Select(pool.maxfd+1, &pool.ready_set, NULL, NULL, NULL);

                /* If listening descriptor ready, add new client to pool */
                if (FD_ISSET(listenfd, &pool.ready_set)) {
                        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
                        add_client(connfd, &pool);
                }

                /* Echo a text line from each ready connected descriptor */
                check_client(&pool);
        }
}



void init_pool(int listenfd, pool *p)
{
        /* Initially, there are no connected descriptors */
        int i;
        p->maxi = -1;
        printf("%d is ready\n", FD_SETSIZE);
        for (i = 0; i < FD_SETSIZE; i++)
                p->clientfd[i] = -1;

        /* Initially, listenfd is only member of select read set */
        p->maxfd = listenfd;
        FD_ZERO(&p->read_set);
        FD_SET(listenfd, &p->read_set);
}

void add_client(int connfd, pool *p)
{
        int i;
        p->nready--;
        for (i = 0; i < FD_SETSIZE; i++) /* Find an available slot */
                if (p->clientfd[i] < 0) {
                        /* Add connected descriptor to the pool */
                        p->clientfd[i] = connfd;
                        Rio_readinitb(&p->clientrio[i], connfd);

                        /* Add the descriptor to descriptor set */
                        FD_SET(connfd, &p->read_set);

                        /* Update max descriptor and pool highwater mark */
                        if (connfd > p->maxfd)
                                p->maxfd = connfd;
                        if (i > p->maxi)
                                p->maxi = i;
                        break;
                }
        if (i == FD_SETSIZE)    /* Couldn't find an empty slot */
                app_error("add_client error: Too many clients");
}

void check_client(pool *p)
{
        int i, n;
        int is_static;
        struct stat sbuf;
        char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
        char filename[MAXLINE], cgiargs[MAXLINE];

        for (i = 0; (i <= p->maxi) && (p->nready > 0); i++) {
                int fd = p->clientfd[i];
                rio_t rio = p->clientrio[i];
                /* If the descriptor is ready, echo a text line from it */
                if ((fd > 0) && (FD_ISSET(fd, &p->ready_set))) {
                        p->nready--;

                    // TODO: 采用更加细粒度的并发设计，每次服务一行
                    /* Read request line and headers */
                    Rio_readinitb(&rio, fd);         
                    
                    if ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
                            printf("%d is under service...\n", fd);
                            sscanf(buf, "%s %s %s", method, uri, version);
                            if (strcasecmp(method, "GET")) {
                                    clienterror(fd, method, "501", "Not implemented",
                                            "Tiny does not implement this method");
                                    return;
                            }
                            // read_requesthdrs(&rio); // 仅仅用于显示headers的信息，目前没有起到实际的作用

                            /* Parse URI from GET request */
                            is_static = parse_uri(uri, filename, cgiargs);
                            if (stat(filename, &sbuf) < 0) {    // 不能找打这个文件
                                    clienterror(fd, filename, "404", "Not found",
                                                "Tiny couldn't read the file");
                                    return;
                            }

                            if (is_static) {        /* Serve static content */
                                    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) { // 是一个普通文件，并且有读的权限吗？
                                            clienterror(fd, filename, "403", "Forbidden",
                                                        "Tiny couldn't read the filetype");
                                            return;
                                    } 
                                    printf("static-serve\n");
                                    serve_static(fd, filename, sbuf.st_size);
                            }
                            else {                  /* Serve dynamic content */
                                    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) { // 是一个普通文件，并且有写的权限吗？
                                            clienterror(fd, filename, "403", "Forbidden",
                                                        "Tiny couldn't run the CGI program");
                                            return;
                                    }
                                    printf("dynamic-serve\n");
                                    serve_dynamic(fd, filename, cgiargs);
                            }
                    }

                    /* EOF detected, remove descriptor from pool */
                    else {
                            Close(fd);
                            FD_CLR(fd, &p->read_set);
                            p->clientfd[i] = -1;
                            printf("%d is down...\n", fd);
                    }

                }
        }
}


// 给定错误信息，转化成网页的形式输出
void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg)
{
        char buf[MAXLINE], body[MAXBUF];

        /* Build the HTTP response body */
        sprintf(body, "<html><title>Tiny Error</title>");
        sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
        sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
        sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
        sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

        /* Print the HTTP response */
        sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
        Rio_writen(fd, buf, strlen(buf));
        sprintf(buf, "Content-type: text/html\r\n");
        Rio_writen(fd, buf, strlen(buf));
        sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
        Rio_writen(fd, buf, strlen(buf));
        Rio_writen(fd, body, strlen(body));
}

void read_requesthdrs(rio_t *rp)
{
        printf("reading the left headers\n");
        char buf[MAXLINE];

        Rio_readlineb(rp, buf, MAXLINE);
        while (strcmp(buf, "\r\n")) {
                Rio_readlineb(rp, buf, MAXLINE);
                printf("%s", buf);
        }
        return;
}

int parse_uri(char *uri, char *filename, char *cgiargs)
{
        char *ptr;

        if (!strstr(uri, "cgi-bin")) { /* Static content */
                strcpy(cgiargs, "");
                strcpy(filename, ".");
                strcat(filename, uri);
                if (uri[strlen(uri)-1] == '/')
                        strcat(filename, "home.html");
                return 1;
        }
        else {                  /* Dynamic content */
                ptr = index(uri, '?');
                if (ptr) {
                        strcpy(cgiargs, ptr+1);
                        *ptr = '\0';
                }
                else
                        strcpy(cgiargs, "");
                strcpy(filename, ".");
                strcat(filename, uri);
                return 0;
        }
}

void serve_static(int fd, char *filename, int filesize)
{
        int srcfd;
        char *srcp, filetype[MAXLINE], buf[MAXBUF];

        /* Send response headers to client */
        get_filetype(filename, filetype);
        sprintf(buf, "HTTP/1.0 200 OK\r\n");
        sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
        sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
        sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
        Rio_writen(fd, buf, strlen(buf));
		
    	// 向client发送body
        /* Send response body to client */
        
        /*srcfd = Open(filename, O_RDONLY, 0);
        srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
        Close(srcfd);
        Rio_writen(fd, srcp, filesize);
        Munmap(srcp, filesize);*/
		
    	srcfd = Open(filename, O_RDONLY, 0);
        char *body = NULL;
        const int min_size = (1<<10); /* 1KB */
        int max_size = (unsigned)(0x8000) >> 1; /* 32bit: 1GB */
        size_t size = min_size;
        for (;;) {
                
                char *old_body = body;
                body = realloc(old_body, size);
                if (body == NULL) { 
                	free(old_body);
                    perror("realloc");
                }
                
                int n = Rio_readn(srcfd, body, size);
                if (n > 0)      /* read something */
                        Rio_writen(fd, body, n);
                if (n != size)  /* EOF or read all the content */
                        break;
                size <<= 1; /* increase buffer, read more next time */
        }

        /* Clean */
        free(body);
}

/*
 * get_filetype - derive file type from name
 */
void get_filetype(char *filename, char *filetype)
{
        if (strstr(filename, ".html"))
                strcpy(filetype, "text/html");
        else if (strstr(filename, ".gif"))
                strcpy(filetype, "image/gif");
        else if (strstr(filename, ".jpg"))
                strcpy(filetype, "image/jpeg");
		else if (strstr(filename, ".mpg"))
                strcpy(filetype, "video/mpeg");
        else
                strcpy(filetype, "text/plain");
}

void serve_dynamic(int fd, char *filename, char *cgiargs)
{
        char buf[MAXLINE], *emptylist[] = { NULL };

        /* Return first part of HTTP response */
        sprintf(buf, "HTTP/1.0 200 OK\r\n");
        Rio_writen(fd, buf, strlen(buf));
        sprintf(buf, "Server: Tiny Web Server\r\n");
        Rio_writen(fd, buf, strlen(buf));

        if (Fork() == 0) {      /* child */
                /* Real server would set all CGI vars here */
                setenv("QUERY_STRING", cgiargs, 1);
                Dup2(fd, STDOUT_FILENO); /* Redirect stdout to client */
                Execve(filename, emptylist, environ); /* Run CGI program */
        }
        Wait(NULL);             /* Parent waits for and reaps child */
}
