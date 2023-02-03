# CS:APP Malloc Lab

## 简介

这个lab要求我们实现c语言中的`molloc`和`free`，以管理内存。

我们需要设计一个高效的管理字节数组内容的数据结构，其实质在进程运行时就是进程的堆空间。

1. 对于空闲块和分配块，应该使用何种基础数据结构？
2. 如何分配块？如何释放块？
3. 如何合并相邻的空闲块？该在什么时候合并？

需要修改的文件是`mm.c`

```sh
# build
make clean && make
# test
make && ./mdriver -t ../traces/ -v
# debug
make && gdb --args ./mdriver -t ../traces/ -v
```

## 设计

根据书上的介绍，我们有下面四种结构可供选择：

隐式链表： 用一位来表示是否分配即可，分配块 ${O(n)}$ ，释放 ${O(1)}$ （教材已实现）
显式链表： 用链表来将所有的空闲块串起来，分配块需要 $O(空闲块数量)$ ，释放为 $O(1)$
分离适配： 维护一个空闲链表的数组
分离存储： 用一个个装等价类的链表来存储空闲块， 分配块平均可以达到 ${O(long 空闲块数量)}$ ，释放为 $O(1)$

另有红黑树高分实现。

## reference

1. [CSAPP: MallocLab多个链表版本的实现与推荐](https://zhuanlan.zhihu.com/p/457373110)
2. [CSAPP | Lab8-Malloc Lab 深入解析](https://zhuanlan.zhihu.com/p/496366818)