## 前置知识

## 准备工作

本实验依赖 `valgrind` 

```
$ sudo apt install valgrind
$ valgrind --version
valgrind-3.15.0
```

## Part A : Writing A Cache Simulator

在这个part里需要在 `csim.c` 里编写一个使用LRU策略的 cache 模拟器，要求最终测试结果要和文件中给出的 `csim-ref` 文件的结果一样。

这个模拟器有6个参数

```bash
Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>
    • -h: Optional help flag that prints usage info
    • -v: Optional verbose flag that displays trace info
    • -s <s>: Number of set index bits (S = 2s is the number of sets)
    • -E <E>: Associativity (number of lines per set)
    • -b <b>: Number of block bits (B = 2b is the block size)
    • -t <tracefile>: Name of the valgrind trace to replay
```

## reference

1. [CSAPP实验之cache lab](https://zhuanlan.zhihu.com/p/79058089)