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

set的意义：加一个层级[提高检索效率](https://www.bilibili.com/video/BV1LP4y1m7Mj?p=12)，相当于缩小hash长度，增加替换深度。

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

具体代码可以在[这里](csim.c)查看，提供了详细的注释。

通过全部测试用例：

```bash
$ ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace
miss miss hit hit hit miss miss hit hit hits:5 misses:4 evictions:0
$ ./csim -s 1 -E 1 -b 1 -t traces/yi2.trace
hits:9 misses:8 evictions:6
$ ./csim-ref -s 1 -E 1 -b 1 -t traces/yi2.trace
hits:9 misses:8 evictions:6
$ ./csim -s 4 -E 2 -b 4 -t traces/yi.trace
hits:4 misses:5 evictions:2
$ ./csim-ref -s 4 -E 2 -b 4 -t traces/yi.trace
hits:4 misses:5 evictions:2
$ ./csim -s 2 -E 1 -b 4 -t traces/dave.trace
hits:2 misses:3 evictions:1
$ ./csim-ref -s 2 -E 1 -b 4 -t traces/dave.trace
hits:2 misses:3 evictions:1
$ ./csim -s 2 -E 1 -b 3 -t traces/trans.trace
hits:167 misses:71 evictions:67
$ ./csim-ref -s 2 -E 1 -b 3 -t traces/trans.trace
hits:167 misses:71 evictions:67
$  ./csim -s 2 -E 2 -b 3 -t traces/trans.trace
hits:201 misses:37 evictions:29
$  ./csim-ref -s 2 -E 2 -b 3 -t traces/trans.trace
hits:201 misses:37 evictions:29
$ ./csim -s 2 -E 4 -b 3 -t traces/trans.trace
hits:212 misses:26 evictions:10
$ ./csim-ref -s 2 -E 4 -b 3 -t traces/trans.trace
hits:212 misses:26 evictions:10
$  ./csim -s 5 -E 1 -b 5 -t traces/trans.trace
hits:231 misses:7 evictions:0
$  ./csim-ref -s 5 -E 1 -b 5 -t traces/trans.trace
hits:231 misses:7 evictions:0
$  ./csim -s 5 -E 1 -b 5 -t traces/long.trace
hits:265189 misses:21775 evictions:21743
$  ./csim-ref -s 5 -E 1 -b 5 -t traces/long.trace
hits:265189 misses:21775 evictions:21743

$ ./test-csim
                        Your simulator     Reference simulator
Points (s,E,b)    Hits  Misses  Evicts    Hits  Misses  Evicts
     3 (1,1,1)       9       8       6       9       8       6  traces/yi2.trace
     3 (4,2,4)       4       5       2       4       5       2  traces/yi.trace
     3 (2,1,4)       2       3       1       2       3       1  traces/dave.trace
     3 (2,1,3)     167      71      67     167      71      67  traces/trans.trace
     3 (2,2,3)     201      37      29     201      37      29  traces/trans.trace
     3 (2,4,3)     212      26      10     212      26      10  traces/trans.trace
     3 (5,1,5)     231       7       0     231       7       0  traces/trans.trace
     6 (5,1,5)  265189   21775   21743  265189   21775   21743  traces/long.trace
    27

TEST_CSIM_RESULTS=27
```

## Part B: Optimizing Matrix Transpose

具体代码可以在[这里](trans.c)查看，提供了详细的注释。

```bash
./test-trans -M 32 -N 32

Function 0 (1 total)
Step 1: Validating and generating memory traces
Step 2: Evaluating performance (s=5, E=1, b=5)
func 0 (Transpose submission): hits:1766, misses:287, evictions:255

Summary for official submission (func 0): correctness=1 misses=287

TEST_TRANS_RESULTS=1:287
$ ./test-trans -M 64 -N 64

Function 0 (1 total)
Step 1: Validating and generating memory traces
Step 2: Evaluating performance (s=5, E=1, b=5)
func 0 (Transpose submission): hits:9066, misses:1179, evictions:1147

Summary for official submission (func 0): correctness=1 misses=1179

TEST_TRANS_RESULTS=1:1179
$ ./test-trans -M 61 -N 67

Function 0 (1 total)
Step 1: Validating and generating memory traces
Step 2: Evaluating performance (s=5, E=1, b=5)
func 0 (Transpose submission): hits:6229, misses:1950, evictions:1918

Summary for official submission (func 0): correctness=1 misses=1950

TEST_TRANS_RESULTS=1:1950
```

## reference

1. [Cache Lab Implementation and Blocking](Cache_Lab_Implementation_and_Blocking.pdf)
2. [CSAPP实验之cache lab](https://zhuanlan.zhihu.com/p/79058089)
3. [《深入理解计算机系统》配套实验：Cache Lab](https://zhuanlan.zhihu.com/p/33846811)
4. [CSAPP-Lab05 Cache Lab 深入解析](https://zhuanlan.zhihu.com/p/484657229)
5. [请教CPU的cache中关于line,block,index等的理解？](https://www.zhihu.com/question/24612442/answer/156669729)
6. [CSAPP: Cachelab全注释+思路和建议](https://zhuanlan.zhihu.com/p/456858668)