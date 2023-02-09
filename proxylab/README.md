# CS:APP Proxy Lab

Web Proxy 是 Web 浏览器和服务器之间的一个中间程序。我们日常使用浏览器访问某个网站时，浏览器不是直接请求服务器来获取内容的，而是联系 Proxy，Proxy 转发请求到服务器，服务器回复 Proxy 后，Proxy 再将回复发送到浏览器。

原始的客户端-服务器:

![](https://pic2.zhimg.com/80/v2-582489a0002c02fda708f70b6af96129_720w.webp)

通过代理的结构:

![](https://pic1.zhimg.com/80/v2-a8475e35ee95a7e213c6339da92b1f38_720w.webp)

本实验就是要实现一个支持多线程带缓存的 Web 代理，分为三个部分来进行：

- Part 1：实现一个最基础的顺序代理
- Part 2：进一步优化，使代理支持多线程（生产者-消费者模型）
- Part 3：使用 LRU 策略缓存 Web 中的对象（读者-写者模型）

## 文件结构说明

This directory contains the files you will need for the CS:APP Proxy
Lab.

proxy.c
csapp.h
csapp.c
    These are starter files.  csapp.c and csapp.h are described in
    your textbook. 

    You may make any changes you like to these files.  And you may
    create and handin any additional files you like.

    Please use `port-for-user.pl' or 'free-port.sh' to generate
    unique ports for your proxy or tiny server. 

Makefile
    This is the makefile that builds the proxy program.  Type "make"
    to build your solution, or "make clean" followed by "make" for a
    fresh build. 

    Type "make handin" to create the tarfile that you will be handing
    in. You can modify it any way you like. Your instructor will use your
    Makefile to build your proxy from source.

port-for-user.pl
    Generates a random port for a particular user
    usage: ./port-for-user.pl <userID>

free-port.sh
    Handy script that identifies an unused TCP port that you can use
    for your proxy or tiny. 
    usage: ./free-port.sh

driver.sh
    The autograder for Basic, Concurrency, and Cache.        
    usage: ./driver.sh

nop-server.py
     helper for the autograder.         

tiny
    Tiny Web server from the CS:APP text

## TINY Web 服务器

代理相对于客户端来说充当了服务端的角色，相对于服务端来说又充当了客户端的角色，所以可以大量利用书中讲解服务器的代码。

CSAPP 课本用 250 行代码(`tiny.c`)为我们实现了一个非常简单的 Web 服务器，在做实验之前，深刻理解它是非常重要的。

### 解析 HTTP 请求

HTTP 请求的组成是这样的：一个请求行，后面跟随零个或更多个请求报头，在跟随一个空的文本行来终止报头列表。请求行的形式是：method URI version，服务器只支持 GET 方法。将 URI 解析出来后，需要判断请求的是静态内容还是动态内容，分别执行。

```c
/*
 * doit - handle one HTTP request/response transaction
 */
/* $begin doit */
void doit(int fd) {
  int is_static;
  struct stat sbuf;
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  char filename[MAXLINE], cgiargs[MAXLINE];
  rio_t rio;

  /* Read request line and headers */
  Rio_readinitb(&rio, fd);
  if (!Rio_readlineb(&rio, buf, MAXLINE))  // line:netp:doit:readrequest
    return;
  printf("%s", buf);
  
  // 解析请求行

  sscanf(buf, "%s %s %s", method, uri, version);  // line:netp:doit:parserequest
  
  // 处理get方法
  
  if (strcasecmp(method, "GET")) {  // line:netp:doit:beginrequesterr
    clienterror(fd, method, "501", "Not Implemented",
                "Tiny does not implement this method");
    return;
  }                        // line:netp:doit:endrequesterr
  read_requesthdrs(&rio);  // line:netp:doit:readrequesthdrs

  /* Parse URI from GET request */
  is_static = parse_uri(uri, filename, cgiargs);  // line:netp:doit:staticcheck
  if (stat(filename, &sbuf) < 0) {  // line:netp:doit:beginnotfound
    clienterror(fd, filename, "404", "Not found",
                "Tiny couldn't find this file");
    return;
  }  // line:netp:doit:endnotfound

  if (is_static) { /* Serve static content */
    if (!(S_ISREG(sbuf.st_mode)) ||
        !(S_IRUSR & sbuf.st_mode)) {  // line:netp:doit:readable
      clienterror(fd, filename, "403", "Forbidden",
                  "Tiny couldn't read the file");
      return;
    }
    // 提供静态内容
    serve_static(fd, filename, sbuf.st_size);  // line:netp:doit:servestatic
  } else {                                     /* Serve dynamic content */
    if (!(S_ISREG(sbuf.st_mode)) ||
        !(S_IXUSR & sbuf.st_mode)) {  // line:netp:doit:executable
      clienterror(fd, filename, "403", "Forbidden",
                  "Tiny couldn't run the CGI program");
      return;
    }
    serve_dynamic(fd, filename, cgiargs);  // line:netp:doit:servedynamic
  }
}
```

### 提供静态内容

一个 HTTP 响应的组成是这样的：一个响应行，后面跟随着零个或更多的相应报头，再跟随着一个终止报头的空行，随后跟随响应主体，相应行的格式为`version status-code status-massage`。

```c
void serve_static(int fd, char *filename, int filesize) {
  int srcfd;
  char *srcp, filetype[MAXLINE], buf[MAXBUF];

  /* Send response headers to client 
     发送响应行给客户端
  */
  get_filetype(filename, filetype);     // line:netp:servestatic:getfiletype
  sprintf(buf, "HTTP/1.0 200 OK\r\n");  // line:netp:servestatic:beginserve

  // 响应报头
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Server: Tiny Web Server\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-length: %d\r\n", filesize);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: %s\r\n\r\n", filetype);
  Rio_writen(fd, buf, strlen(buf));  // line:netp:servestatic:endserve

  /* Send response body to client */
  // 获得文件描述符
  srcfd = Open(filename, O_RDONLY, 0);  // line:netp:servestatic:open
  srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd,
              0);                  // line:netp:servestatic:mmap
  // 映射到内存
  Close(srcfd);                    // line:netp:servestatic:close
  // 文件传送
  Rio_writen(fd, srcp, filesize);  // line:netp:servestatic:write
  // 释放内存
  Munmap(srcp, filesize);          // line:netp:servestatic:munmap
}
```

### 提供动态内容

TINY 派生一个子进程并在子进程的上下文中运行一个 CGI 程序

```c
void serve_dynamic(int fd, char *filename, char *cgiargs) {
  char buf[MAXLINE], *emptylist[] = {NULL};

  /* Return first part of HTTP response */
  sprintf(buf, "HTTP/1.0 200 OK\r\n");  // 响应行
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Server: Tiny Web Server\r\n");  // 响应头
  Rio_writen(fd, buf, strlen(buf));

  if (Fork() == 0) { /* Child */  // line:netp:servedynamic:fork
    /* Real server would set all CGI vars here */
    // URI 参数初始化环境变量
    setenv("QUERY_STRING", cgiargs, 1);  // line:netp:servedynamic:setenv
    Dup2(fd, STDOUT_FILENO);
    /* Redirect stdout to client */  // line:netp:servedynamic:dup2
    Execve(filename, emptylist, environ);
    /* Run CGI program */  // line:netp:servedynamic:execve
  }
  Wait(NULL);
  /* Parent waits for and reaps child */  // line:netp:servedynamic:wait
}
```

### 主函数

```c
int main(int argc, char **argv) {
  int listenfd, connfd;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  /* Check command line args */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  listenfd = Open_listenfd(argv[1]);  // 打开监听描述符
  while (1) {
    clientlen = sizeof(clientaddr);
    // 接受请求
    connfd = Accept(listenfd, (SA *)&clientaddr,
                    &clientlen);  // line:netp:tiny:accept
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE,
                0);
    printf("Accepted connection from (%s, %s)\n", hostname, port);
    // 执行
    doit(connfd);   // line:netp:tiny:doit
    Close(connfd);  // line:netp:tiny:close
  }
}
```

## Part 1: Implementing A sequential web proxy

要求：

1. 实现一个处理 HTTP/1.0 GET 请求的基本顺序 web 代理
2. 如果接收到一个浏览器请求为 HTTP/1.1 版本，则应将它作为 HTTP/1.0 请求转发
3. 转发合法的 HTTP 请求
    - 假设请求为 GET http://www.cmu.edu/hub/index.html HTTP/1.1
    - 则主机名为 www.cmu.edu
    - 请求的页面为 /hub/index.html
    - HTTP 请求每行以 \r\n 结束，以一个空行 \r\n 结尾
5. 代理发送的请求的 header 为：
    - Host: 如 Host: www.cmu.edu
    - User-Agent: 如 User-Agent: Mozilla/5.0 (X11; Linux  x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3
    - Connection: 必须发送 Connection: close
    - Proxy-Connection: 必须发送 Proxy-Connection: close
要处理过早关闭的连接，即必须捕获 SIGPIPE 信号

```c
#include <stdio.h>

#include "csapp.h"
/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

struct Uri {
  char host[MAXLINE];  // hostname
  char port[MAXLINE];  // 端口
  char path[MAXLINE];  // 路径
};

void doit(int connfd);
void parse_uri(char *uri, struct Uri *uri_data);
void build_header(char *http_header, struct Uri *uri_data, rio_t *client_rio);

/* 处理SIGPIPE信号 */
void sigpipe_handler(int sig) {
  printf("haha?");
  return;
}

int main(int argc, char **argv) {  // 代码与 TINY服务器代码完全相同
  int listenfd, connfd;
  socklen_t clientlen;
  char hostname[MAXLINE], port[MAXLINE];

  struct sockaddr_storage clientaddr;
  if (argc != 2) {
    fprintf(stderr, "usage :%s <port> \n", argv[0]);
    exit(1);
  }
  signal(SIGPIPE, sigpipe_handler);
  listenfd = Open_listenfd(argv[1]);
  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE,
                0);
    printf("Accepted connection from (%s %s).\n", hostname, port);

    doit(connfd);
    // 关闭客户端的连接描述符
    Close(connfd);
  }
  return 0;
}

void doit(int connfd) {
  // doit函数前半部分与 TINY 服务器相似
  // 不同的是将后半部分的请求处理改为转发给服务器，再将服务器的回复回复给客户端
  
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  char server[MAXLINE];

  rio_t rio, server_rio;

  Rio_readinitb(&rio, connfd);
  Rio_readlineb(&rio, buf, MAXLINE);
  sscanf(buf, "%s %s %s", method, uri, version);

  if (strcasecmp(method, "GET")) {
    printf("Proxy does not implement the method");
    return;
  }

  struct Uri *uri_data = (struct Uri *)malloc(sizeof(struct Uri));
  // 解析uri
  parse_uri(uri, uri_data);

  // 设置header
  build_header(server, uri_data, &rio);

  // 连接服务器
  int serverfd = Open_clientfd(uri_data->host, uri_data->port);
  if (serverfd < 0) {
    printf("connection failed\n");
    return;
  }

  Rio_readinitb(&server_rio, serverfd);
  Rio_writen(serverfd, server, strlen(server));

  size_t n;
  while ((n = Rio_readlineb(&server_rio, buf, MAXLINE)) != 0) {
    printf("proxy received %d bytes,then send\n", (int)n);
    Rio_writen(connfd, buf, n);
  }
  Close(serverfd);
}

void build_header(char *http_header, struct Uri *uri_data, rio_t *client_rio) {
  char *User_Agent =
      "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
      "Firefox/10.0.3\r\n";
  char *conn_hdr = "Connection: close\r\n";
  char *prox_hdr = "Proxy-Connection: close\r\n";
  char *host_hdr_format = "Host: %s\r\n";
  char *requestlint_hdr_format = "GET %s HTTP/1.0\r\n";
  char *endof_hdr = "\r\n";

  char buf[MAXLINE], request_hdr[MAXLINE], other_hdr[MAXLINE],
      host_hdr[MAXLINE];
  sprintf(request_hdr, requestlint_hdr_format, uri_data->path);
  while (Rio_readlineb(client_rio, buf, MAXLINE) > 0) {
    if (strcmp(buf, endof_hdr) == 0) break; /*EOF*/

    if (!strncasecmp(buf, "Host", strlen("Host"))) { /*Host:*/
      strcpy(host_hdr, buf);
      continue;
    }

    if (!strncasecmp(buf, "Connection", strlen("Connection")) &&
        !strncasecmp(buf, "Proxy-Connection", strlen("Proxy-Connection")) &&
        !strncasecmp(buf, "User-Agent", strlen("User-Agent"))) {
      strcat(other_hdr, buf);
    }
  }
  if (strlen(host_hdr) == 0) {
    sprintf(host_hdr, host_hdr_format, uri_data->host);
  }
  sprintf(http_header, "%s%s%s%s%s%s%s", request_hdr, host_hdr, conn_hdr,
          prox_hdr, User_Agent, other_hdr, endof_hdr);

  return;
}

// 解析uri
void parse_uri(char *uri, struct Uri *uri_data) {
  char *hostpose = strstr(uri, "//");
  if (hostpose == NULL) {
    char *pathpose = strstr(uri, "/");
    if (pathpose != NULL) strcpy(uri_data->path, pathpose);
    strcpy(uri_data->port, "80");
    return;
  } else {
    char *portpose = strstr(hostpose + 2, ":");
    if (portpose != NULL) {
      int tmp;
      sscanf(portpose + 1, "%d%s", &tmp, uri_data->path);
      sprintf(uri_data->port, "%d", tmp);
      *portpose = '\0';
    } else {
      char *pathpose = strstr(hostpose + 2, "/");
      if (pathpose != NULL) {
        strcpy(uri_data->path, pathpose);
        strcpy(uri_data->port, "80");
        *pathpose = '\0';
      }
    }
    strcpy(uri_data->host, hostpose + 2);
  }
  return;
}
```

## Part 2: Dealing with multiple concurrent requests

![](https://pic3.zhimg.com/80/v2-c53d48a2489d0fa299c88998755645b6_720w.webp)

### 生产者-消费者模型

生产者和消费者线程共享一个有 n 个槽的优先缓冲区。

- 生产者反复地生成新的项目，并把它们插入到缓冲区中。
- 消费者线程不断从缓冲区中取出，这些项目然后使用它们。

因为插入和取出项目都涉及更新共享变量，所以我们必须保证对缓冲区地访问是互斥的，并调度对缓冲区地访问：

- 如果没有槽位：生产者必须等待
- 如果没有项目：消费者必须等待

在预线程化的服务器中，客户端就是生产者，不断产生连接；Proxy 就是消费者，不断选中客户端连接。

为此，开发一个 SBUF 包来实现这个缓冲区地调度。缓冲区设计：

```c
typedef struct {
  int *buf;    /* Buffer array */
  int n;       /* Maximum number of slots */
  int front;   /* buf[(front+1)%n] is first item */
  int rear;    /* buf[rear%n] is last item */
  sem_t mutex; /* Protects accesses to buf */
  sem_t slots; /* Counts available slots */
  sem_t items; /* Counts available items */
} sbuf_t;
```

其中有 3 个信号量：
- mutex 初始化为 1，保证对缓冲区的访问是互斥的
- slots 初始化为 n，记录槽位
- items 初始化为 0，记录项目数

由此，设计客户端向缓冲区插入函数：

```c
void sbuf_insert(sbuf_t *sp, int item) {
  P(&sp->slots);                          /* Wait for available slot */
  P(&sp->mutex);                          /* Lock the buffer */
  sp->buf[(++sp->rear) % (sp->n)] = item; /* Insert the item */
  V(&sp->mutex);                          /* Unlock the buffer */
  V(&sp->items);                          /* Announce available item */
}
```

- 首先有一个对 slots 的 P 操作，保证了如果槽位已满，则客户端被挂起，不会继续往缓冲区写入请求
- 然后是一个 mutex 的 P 操作，保证了对缓冲区互斥的访问
- 缓冲区插入完成后，释放所有的锁

 Proxy 从缓冲区读入的函数：

```c
int sbuf_remove(sbuf_t *sp) {
  int item;
  P(&sp->items);                           /* Wait for available item */
  P(&sp->mutex);                           /* Lock the buffer */
  item = sp->buf[(++sp->front) % (sp->n)]; /* Remove the item */
  V(&sp->mutex);                           /* Unlock the buffer */
  V(&sp->slots);                           /* Announce available slot */
  return item;
}
```

- 首先有一个对 items 的 P 操作，保证了如果没有项目，则 Proxy 被挂起，不会继续往缓冲区读出客户端
- 然后是一个 mutex 的 P 操作，保证了对缓冲区互斥的访问
- 缓冲区读出完成后，释放所有的锁，返回其中一个客户端的描述符

线程执行函数：

```c
void *thread(void *vargp) {
  Pthread_detach(pthread_self());
  while (1) {
    // 从缓冲区中读出描述符
    int connfd = sbuf_remove(&sbuf);
    // 转发
    doit(connfd);
    // 关闭客户端的连接描述符
    Close(connfd);
  }
}
```

## Part 3: Caching web objects

在 Proxy 中，当多个客户端或一个客户端多次访问同一个服务端的同一对象时，Proxy 每次都要从服务端请求，这是很耗费时间的。如果 Proxy 能把访问过的对象存储下来，那么再次遇到同样的请求时，就不需要再连接到服务端了，可以直接回复给客户端。而 Cache 的大小并不是无限的，所以就又要考虑替换策略，本实验要求使用 LRU。

### 读者-写者模型

一组并发的线程要访问同一个共享对象时，将只读对象的线程叫做读者，只修改对象的进程叫做写者。写者必须拥有对对象的独占的访问，而读者可以与其它读者共享对象。该模型分为两类：

- 读优先：要求不让读者等待
- 写优先：要求在写者之后到达的读者必须等待

对于读者优先，只要有其它读者，则就应让读者先读，因此设置一个 read_cnt 记录读者数量，这个变量对于读者来说也是互斥的，所以设置一个 mutex 信号量来保护，读的过程中不允许写者写，写的过程不允许读者读，所以设置 w 来保护共享对象。

读优先代码：

```c
int read_cnt;
sem_t mutex, w;  // 都初始化为1

void reader(void) {
  while (1) {
    P(&mutex);
    readcnt++;
    if (readcnt == 1) P(&w);
    V(&mutex);

    P(&mutex);
    readcnt--;
    if (readcnt == 0) V(&w);
    V(&mutex);
  }
}

void writer(void) {
  while (1) {
    P(&w);

    //...

    V(&w)
  }
}
```

只有第一个读者对 w 加锁，最后一个读者解锁。因此一旦有一个读者加锁，写者就无法访问这个共享对象了，而其它读者可以随意访问

在本实验中，对 Cache 的访问就是一个经典的读者-写者问题。有的线程要从 Cache 中读出，有的线程要写入 Cache。显然，为了 Proxy 更加高效，这里应该用读优先的模型。

[最终代码](./proxy.c)。

## reference

1. [CSAPP | Lab9-Proxy Lab 深入解析](https://zhuanlan.zhihu.com/p/497982541)