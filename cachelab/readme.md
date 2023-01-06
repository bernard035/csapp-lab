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

通过全部测试用例

```bash
 ⚡ root  ~/csapp-lab/cachelab   main ±  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace
miss miss hit hit hit miss miss hit hit hits:5 misses:4 evictions:0
 ⚡ root  ~/csapp-lab/cachelab   main ±  ./csim -s 1 -E 1 -b 1 -t traces/yi2.trace
hits:9 misses:8 evictions:6
 ⚡ root  ~/csapp-lab/cachelab   main ±  ./csim-ref -s 1 -E 1 -b 1 -t traces/yi2.trace
hits:9 misses:8 evictions:6
 ⚡ root  ~/csapp-lab/cachelab   main ±  ./csim -s 4 -E 2 -b 4 -t traces/yi.trace
hits:4 misses:5 evictions:2
 ⚡ root  ~/csapp-lab/cachelab   main ±  ./csim-ref -s 4 -E 2 -b 4 -t traces/yi.trace
hits:4 misses:5 evictions:2
 ⚡ root  ~/csapp-lab/cachelab   main ±  ./csim -s 2 -E 1 -b 4 -t traces/dave.trace
hits:2 misses:3 evictions:1
 ⚡ root  ~/csapp-lab/cachelab   main ±  ./csim-ref -s 2 -E 1 -b 4 -t traces/dave.trace
hits:2 misses:3 evictions:1
 ⚡ root  ~/csapp-lab/cachelab   main ±  ./csim -s 2 -E 1 -b 3 -t traces/trans.trace
hits:167 misses:71 evictions:67
 ⚡ root  ~/csapp-lab/cachelab   main ±  ./csim-ref -s 2 -E 1 -b 3 -t traces/trans.trace
hits:167 misses:71 evictions:67
 ⚡ root  ~/csapp-lab/cachelab   main ±   ./csim -s 2 -E 2 -b 3 -t traces/trans.trace
hits:201 misses:37 evictions:29
 ⚡ root  ~/csapp-lab/cachelab   main ±   ./csim-ref -s 2 -E 2 -b 3 -t traces/trans.trace
hits:201 misses:37 evictions:29
 ⚡ root  ~/csapp-lab/cachelab   main ±  ./csim -s 2 -E 4 -b 3 -t traces/trans.trace
hits:212 misses:26 evictions:10
 ⚡ root  ~/csapp-lab/cachelab   main ±  ./csim-ref -s 2 -E 4 -b 3 -t traces/trans.trace
hits:212 misses:26 evictions:10
 ⚡ root  ~/csapp-lab/cachelab   main ±   ./csim -s 5 -E 1 -b 5 -t traces/trans.trace
hits:231 misses:7 evictions:0
 ⚡ root  ~/csapp-lab/cachelab   main ±   ./csim-ref -s 5 -E 1 -b 5 -t traces/trans.trace
hits:231 misses:7 evictions:0
 ⚡ root  ~/csapp-lab/cachelab   main ±   ./csim -s 5 -E 1 -b 5 -t traces/long.trace
hits:265189 misses:21775 evictions:21743
 ⚡ root  ~/csapp-lab/cachelab   main ±   ./csim-ref -s 5 -E 1 -b 5 -t traces/long.trace
hits:265189 misses:21775 evictions:21743
```

## reference

1. [Cache Lab Implementation and Blocking](Cache_Lab_Implementation_and_Blocking.pdf)
2. [CSAPP实验之cache lab](https://zhuanlan.zhihu.com/p/79058089)
3. [《深入理解计算机系统》配套实验：Cache Lab](https://zhuanlan.zhihu.com/p/33846811)
4. [CSAPP-Lab05 Cache Lab 深入解析](https://zhuanlan.zhihu.com/p/484657229)
5. [请教CPU的cache中关于line,block,index等的理解？](https://www.zhihu.com/question/24612442/answer/156669729)