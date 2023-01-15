# CS:APP Shell Lab

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