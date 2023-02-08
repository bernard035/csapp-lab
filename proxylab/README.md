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



## reference

1. [CSAPP | Lab9-Proxy Lab 深入解析](https://zhuanlan.zhihu.com/p/497982541)