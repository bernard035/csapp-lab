# CS:APP Shell Lab

## 简介

Shell Lab 要求实现一个带有作业控制的 Unix Shell 程序，需要考虑基础的并发，进程控制以及信号和信号处理。

Students implement their own simple Unix shell program with job control, including the `ctrl-c` and `ctrl-z` keystrokes, `fg`, `bg`, and `jobs` commands. This is the students' first introduction to application level concurrency, and gives them a clear idea of Unix process control, signals, and signal handling.

Files:

```
Makefile	# Compiles your shell program and runs the tests
README		# This file
tsh.c		# The shell program that you will write and hand in
tshref		# The reference shell binary.
```

**The remaining files are used to test your shell**

```
sdriver.pl	# The trace-driven shell driver
trace*.txt	# The 15 trace files that control the shell driver
tshref.out 	# Example output of the reference shell on all 15 traces
```

**Little C programs that are called by the trace files**

```
myspin      # Takes argument <n> and spins for <n> seconds
mysplit.c	# Forks a child that spins for <n> seconds
mystop.c    # Spins for <n> seconds and sends SIGTSTP to itself
myint.c     # Spins for <n> seconds and sends SIGINT to itself
```

我们需要在`tsh.c`中实现函数：

```
eval：解析命令行
builtin_cmd：检测是否为内置命令quit、fg、bg、jobs
do_bgfg：实现内置命令bg和fg
waitfg：等待前台作业执行完成
sigchld_handler：处理SIGCHLD信号，即子进程停止或者终止
sigint_handler：处理SIGINT信号，即来自键盘的中断ctrl-c
sigtstp_handler：处理SIGTSTP信号，即来自终端的停止信号
```

## 设计

### 僵死进程的回收

**一个终止了但是还未被回收的进程称为僵死进程**。对于一个长时间运行的程序（比如 Shell）来说，内核不会安排init进程去回收僵死进程，而它虽不运行却仍然消耗系统资源，因此实验要求我们回收所有的僵死进程。

在 UNIX 系统中，一个进程结束了，但是它的父进程没有等待(调用 wait / waitpid ) 它，那么它将变成一个僵尸进程。在fork()/execve()过程中，假设子进程结束时父进程仍存在，而父进程fork()之前既没安装SIGCHLD信号处理函数调用 waitpid()等待子进程结束，又没有显式忽略该信号，则子进程成为僵尸进程。

### 并发编程原则

1. 注意保存和恢复 errno 。很多函数会在出错返回时设置 errno ，在处理程序中调用这样的函数可能会干扰主程序中其他依赖于 errno 的部分，解决办法是在进入处理函数时用局部变量保存它，运行完成后再将其恢复。
2. 访问全局数据时，阻塞所有信号。
3. 不可以用信号来对其它进程中发生的事情计数。未处理的信号是不排队的，即每种类型的信号最多只能有一个待处理信号。举例：如果父进程将要接受三个相同的信号，当处理程序还在处理一个信号时，第二个信号就会加入待处理信号集合，如果此时第三个信号到达，那么它就会被简单地丢弃，从而出现问题。
4. 注意考虑由竞争导致的同步错误。

### 显式地等待信号

`(fgpid(jobs)`的含义是指通过向<job>对应的作业发送SIGCONT信号来使它重启并放在前台运行


```c
while(fgpid(jobs) != 0)
    pause();
```

如果此处某个父进程调用`(fgpid(jobs)`函数，而此时有一个子进程仍然在前台运行，所以判断条件为真，进入循环，而父进程在进入循环后、执行`pause`前的时间窗口内，前台运行的子进程终止，导致父进程接收到 SIGCHLD 信号。由于`pause`仅在捕捉到信号后返回，而之后不会再有任何信号抵达，那么父进程就会永远休眠。一个解决办法是用sleep函数：因为它不依赖信号来返回，通过每次循环来监测子进程状态。

也可以用sigsuspend函数，这个函数相当于如下代码：

```c
sigprocmask(SIG_SETMASK, &mask, &prev);
pause();
sigprocmask(SIG_SETMASK, &prev, NULL);
```

`sigsuspend`函数相当于同时执行`sigprocmask`和`pause`的原子版本，不会被中断。

所以改为这样就可以了

```c
void waitfg(pid_t pid) {
  sigset_t mask;
  Sigemptyset(&mask);   
  while (fgpid(jobs) != 0){
    sigsuspend(&mask);
  }
  return;
}
```

更多详细实现可以参见

## reference

1. [CSAPP | Lab7-Shell Lab 深入解析](https://zhuanlan.zhihu.com/p/492645370)
2. [CSAPP: Shlab扩展版实现](https://zhuanlan.zhihu.com/p/457057668)