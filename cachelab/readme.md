## 前置知识

![alt](https://uploadfiles.nowcoder.com/images/20230106/794972751_1672952147148/D2B5CA33BD970F64A6301FA75AE2EB22)

1. cache由set组成
2. set由line组成
3. line由valid set、tag和data组成，tag是用于检索cache line的标签，data是需要的内存地址中的数据

内存地址被分成三部分：tag、set index和block offset

![alt](https://uploadfiles.nowcoder.com/images/20230106/794972751_1672960373198/D2B5CA33BD970F64A6301FA75AE2EB22)

cache寻址过程：
1. 根据set index找到set
2. 更趋tag在set定位到cacheline，如果找到了且valid bit为1则为命中，若未命中就根据置换策略找一个cache line从内存加载数据进去
3. 根据block offset在line的data里找到所需的值

set的意义：加一个层级提高检索效率

cache根据寻址方式可以分为3类：
- 直接映射(direct mapped cache) ，相当于每个set只有1个cache line。
- 组关联 (set associative cache) ，多个set, 每个set多个cache line。一般每个set有n个cache line,就说n-ways associative cache。
- 全相联(fully associative cache)，相当于只有1个set


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