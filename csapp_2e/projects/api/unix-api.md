
## 进程控制

- getpid/getppid 获取进程 ID

```
#include <sys/types.h>
#include <unistd.h>

pid_t getpid(void);
pid_t getppid(void);

getpid() 返回进程的 PID;
getppid() 返回父进程的 PID;
```

- exit 终止进程

```
#include <stdlib.h>

void exit(int status);

返回：该函数无返回值
```
- fork 创建进程

```
#include <sys/types.h>
#include <unistd.h>

pid_t fork(void);

返回：子进程返回 0，父进程返回子进程的 PID，如果出错，则为 -1
```

- waitpid/wait 等待子进程的终止或者停止

```
#include <sys/types.h>
#include <sys/wait.h>

pid_t waitpid(pid_t pid, int *status, int options);

返回：如果成功，则为子进程的 PID，如果 WNOHANG，则为 0，如果其他错误，则为 -1

```

pid = -1，目标集合是子进程。
pid > 0，目标是一个单独的进程

option = 0 只要目标集合中有一个终止，返回终止的pid，如果没有了，那么返回-1，阻塞等待。
option = WNOHANG 只要目标集合中有一个没有终止，就返回那个没有终止的pid，否则返回0，不因为等待而阻塞。
option = WUNTRACED 只要目标集合中有一个停止或终止，类似option = 0
opton = 1 | 2
错误条件。

没有子进程，返回-1，errno = ECHILD
如果被一个信号中断，返回-1，errono = EINTR

检查子进程的退出状态

```
WIFEXITED(status);
WEXITSTATUS(status);
WIFSIGNALED(status);
WTERMSIG(status);
WIFSTOPPED(status);
WSTOPSIG(status);
```

wait 函数是 waitpid 函数的简单版本
```
#include <sys/types.h>
#include <sys/wait.h>

pid_t wait(int *status);

返回：如果成功，则为子进程的 PID，如果出错，则为 -1
```
wait(&status) == waitpid(-1, &status, 0);

- sleep/pause 让进程休眠

sleep() 将一个进程挂起一段指定的时间。
```
#include <unistd.h>

unsigned int sleep(unsigned int secs);

返回：还要休眠的秒数
```
pause() 让调用函数休眠，直到该进程收到一个信号。
```
#include <unistd.h>

int pause(void);

返回：总是返回 -1
```

- execve 加载并运行程序

```
#include <unistd.h>

int execve(const char *filename, const char *argv[], const char *envp[]);

返回：如果成功，则不返回；如果错误，则返回 -1
```
- getenv/setenv/unsetenv 操作环境数组

```
#include <stdlib.h>

char *getenv(const char *name);
int setenv(const char *name, const char *newvalue, int overwrite);
void unsetenv(const char *name);

getenv：若存在则为指向 name 的指针，若无匹配，则为 NULL
setenv：若成功返回 0，若错误返回 -1
unsetenv：删除一个name=value的字符串
```
## 信号处理

- getpgrp/setpgid 获取/设置 进程组 ID

```
#include <unistd.h>

pid_t getpgrp(void);
int setpgid(pid_t pid, pid_t pgid);

getpgrp: 返回调用进程的进程组 ID
setpgid: 改变自己或其他进程组的 ID。成功返回 0，失败返回 -1
如果第一个参数是0,那么目标进程就是本进程，如果第二个参数是0,那么目标进程组和目标进程相同。
```

- kill/alarm 发送信号

```
#include <sys/types.h>
#include <signal.h>

int kill(pid_t pid, int sig);

kill: 发送信号给其它进程（包括它自己）。成功返回 0，失败返回 -1
如果pid > 0，那么发送给目标进程，如果 pid < 0，那么发送给目标进程组。
```

```
#include <unistd.h>

unsigned int alarm(unsigned int secs);

alarm: 向自己发送 SIGALARM 信号。返回前一次闹钟剩余的秒数，若以前没有设定闹钟， 则为 0
```

- signal 接收信号

```
#include <signal.h>
typedef void (*sighandler_t)(int);

sighandler_t signal(int signum, sighandler_t handler);

signal: 修改和信号相关联的默认行为。成功则为指向前次处理程序的指针，错误则为SIG_ERR（不设置 errno）
```

- sigaction/sigprocmask 可移植的信号处理

```
#include <signal.h>

int sigaction(int signum, struct sigaction *act, struct sigaction *oldact);
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigaddset(sigset_t *set, int signum);
int sigdelset(sigset_t *set, int signum);
int sigmember(const sigset_t *set, int signum);

sigaction: 同下
sigprocmask: 同下
sigemptyset: 同下
sigfillset: 同下
sigaddset: 同下
sigdelset: 成功返回 0，出错返回 -1
sigmember: 若 signum 是 sef 的成员返回 1，如果不是返回 0，出错返回 -1
```
关于Signal可移植版本的信号处理函数```Signal```的实现，参考```csapp.c```。

- setjmp/sigsetjmp/longjmp/siglongjmp 非本地跳转

```
#include <setjmp.h>

int setjmp(jmp_buf env);
int sigsetjmp(sigjmp_buf env, int savesigs);
void longjmp(jmp_buf env, int retval);
void siglongjmp(sigjmp_buf env, int retval);
sigxxxx是可以被信号处理函数使用的版本。
setjmp: 返回 0
```

```
#include <unistd.h>
#include <sys/mman.h>

void* mmap(void* start, size_t length, int prot, int flags, int fd, off_t offset);
成功时为指向映射区域的地址，出错就是-1

#include <unistd.h>
#include <sys/mman.h>

int munmap(void* start, size_t length);s
成功返回0,出错-1
```

## 文件操作

- open/close

```
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int open(char *filename, int flags, mode_t mode);
open: 若成功则为新文件描述符，若出错为 -1

#include <unistd.h>
int close(int fd);
close: 若成功返回 0，若出错则为 -1
```
unix会将实际的mod设置成 `mode & ~umask`

```
#define DEF_MODE S_IRUSER | ...
#define DEF_UMAKS S_IWGRP | S_IWOTH
```
- read/write

```
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t n);
ssize_t write(int fd, const void *buf, size_t n);

read: 若成功返回读的字节数，若EOF则为0，若出错则为 -1
write: 成功返回写的字节数，出错则为 -1
```

- stat/fstat

```
#include <unistd.h>
#include <sys/stat.h>

int stat(const char *filename, struct stat *buf);
int fstat(int fd, struct stat *buf);

stat: 同下
fstat: 成功返回 0，出错返回 -1
```

- dup/dup2

```
#include <unistd.h>

int dup2(int oldfd, int newfd);

返回：若成功则为非负的描述符，若出错则为 -1
```


## 网络编程

- htonl/htons ntohl/ntohs

```
#include <netinet/in.h>

unsigned long  int htonl(unsigned long  int hostlong);
unsigned short int htons(unsigned short int hostshort);

unsigned long  int ntohl(unsigned long  int netlong);
unsigned short int ntohs(unsigned short int netshort);

htonl/htons: 返回按照网络字节顺序的值
ntohl/ntohs: 返回按照主机字节顺序的值
```
- inet_aton/inet_ntoa


```
#include <netinet/in.h>
#include <arpa/inet.h>

int inet_aton(const char *cp, struct in_addr *inp);
char *inet_ntoa(struct in_addr in);

inet_aton: 将一个点分十进制串转换为一个网络字节顺序的 IP 地址成功返回 1，出错则为 0
inet_ntoa: 将一个网络字节顺序的 IP 地址转换为它所对应的点分十进制返回指向点分十进制字符串的指针
```

- gethostbyname/gethostbyaddr


```
#include <netdb.h>

struct hostent *gethostbyname(const char *name);
struct hostent *gethostbyaddr(const char *addr, int len, 0);

gethostbyname/gethostbyaddr: 成功返回 非NULL 指针，出错则为 NULL，同时设置 h_errno
```

- socket/connect/bind/listen/accept

```
#include <sys/types.h>
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
socket: 成功返回非负描述符，出错返回 -1

例子

clientfd = Socket(AF_INET, SOCKET_STREAM, 0);

```

```
#include <sys/socket.h>
int connect(int sockfd, struct sockaddr *serv_addr, int addrlen);
connect: 成功返回 0，出错返回 -1
```

一般将上述的两个函数包装使用csapp.h。

下面的用于建立客户端和服务器的链接。

```
#include <sys/socket.h>
int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
服务器地址和socketfd连接起来。
int listen(int sockfd, int backlog);
```

一般将上述函数包装起来使用csapp.c。

```
int accept(int listenfd, struct sockaddr *addr, int *addrlen);
bind/listen: 成功返回 0，出错返回 -1
accept: 成功返回非负连接描述符，出错返回 -1
```

## 线程控制
- pthread_create 创建线程

```
#include <pthread.h>
typedef void *(func)(void *);

int pthread_create(pthread_t *tid, pthread_attr_t *attr, func *f, void *arg);

这个函数创建一个新的线程，并带着一个输入变量 arg，在新线程的上下文中运行线程例
程 f。能用 attr 参数来改变新创建线程的默认属性。详情请参考《UNIX高级编程环境·第2
版》的第 11 章和第 12 章。

返回值：成功返回 0，出错则为非零
```

- pthread_t pthread_self(void);

返回值：调用者的线程 ID

- pthread_exit/pthread_cancel 终止线程

```
#include <pthread.h>

void pthread_exit(void *thread_return);
int pthread_cancel(pthread_t tid);

返回值：成功返回 0，出错返回非零
```

- pthread_join 回收已终止线程的资源

```
#include <pthread.h>

int pthread_cancel(pthread_t pid, void **thread_return);

```

返回值：成功返回 0，出错返回非零

这个函数等待其他线程终止。它会阻塞，直到线程 tid 终止，将线程例程返回的
(void *) 指针赋值为 thread_return 指向的位置，然后回收已终止线程占用的所有存储器
资源。

pthread_join() 只能等待一个指定的线程终止，没有办法让 pthread_join() 等待任意一个
线程终止。


- pthread_detach 分离线程

在任何一个时间点上，线程是可结合的或者是可分离的。一个分离的线程是不能被其他线程
回收或杀死的，它的存储器资源在它终止时由系统自动释放。

默认情况下，线程被创建成可结合的。一个可结合的线程能够被其他线程回收其资源和杀死；
在被其他线程回收之前，它的存储器资源是没有被释放的。
```
#include <pthread.h>

int pthread_detach(pthread_t pid);

返回值：成功返回 0，出错返回非零
```
- pthread_once 初始化线程

```
#include <pthread.h>

pthread_once_t once_control = PTHREAD_ONCE_INIT;

int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));

返回值：总是返回 0

```
当第一次用参数 once_control 调用 pthread_once() 时，它调用 init_routine，这是一
个没有任何参数也不返回任何值的函数。

当需要动态初始化多个线程共享的全局变量时，pthread_once() 是很有用的。

- 信号量

```
#include <semaphore.h>

int sem_init(sem_t *sem, 0, unsigned int value);
int sem_wait(sem_t *s); /* P(s) */
int sem_post(sem_t *s); /* V(s) */

返回值：若成功返回 0，若出错返回 -1
```

