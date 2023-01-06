/*
 * A Cache Simulator by Bernard
 * 2023/1/6
 */
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cachelab.h"

typedef struct CacheLine_ {
  int valid;      // 有效位
  int tag;        // 标记位
  int time_tamp;  // 时间戳
} CacheLine;

typedef struct Cache_ {
  int S;
  int E;
  int B;
  CacheLine **line;
} Cache;

int hit_count = 0;  // 命中，要操作的数据在对应组的其中一行
int miss_count = 0;  // 不命中次数，要操作的数据不在对应组的任何一行
int eviction_count = 0;  // 驱赶次数，要操作的数据的对应组已满，进行了替换操作

int print_detail = 0;  // 是否打印详细信息

char t[1000];
Cache *cache = NULL;

void InitCache(int s, int E, int b) {
  int S = 1 << s;
  int B = 1 << b;
  cache = (Cache *)malloc(sizeof(Cache));
  cache->S = S;
  cache->E = E;
  cache->B = B;
  cache->line = (CacheLine **)malloc(sizeof(CacheLine *) * S);
  for (int i = 0; i < S; i++) {
    cache->line[i] = (CacheLine *)malloc(sizeof(CacheLine) * E);
    for (int j = 0; j < E; j++) {
      cache->line[i][j].valid = 0;  // 初始时，高速缓存是空的
      cache->line[i][j].tag = -1;
      cache->line[i][j].time_tamp = 0;
    }
  }
}

void FreeCache() {
  int S = cache->S;
  for (int i = 0; i < S; i++) {
    free(cache->line[i]);
  }
  free(cache->line);
  free(cache);
}

int GetIndex(int op_s, int op_tag) {
  for (int i = 0; i < cache->E; i++) {
    if (cache->line[op_s][i].valid && cache->line[op_s][i].tag == op_tag)
      return i;
  }
  return -1;
}

int FindLRU(int op_s) {
  int max_index = 0;
  int max_stamp = 0;
  for (int i = 0; i < cache->E; i++) {
    if (cache->line[op_s][i].time_tamp > max_stamp) {
      max_stamp = cache->line[op_s][i].time_tamp;
      max_index = i;
    }
  }
  return max_index;
}

int IsFull(int op_s) {
  for (int i = 0; i < cache->E; i++) {
    if (cache->line[op_s][i].valid == 0) return i;
  }
  return -1;
}

void Update(int i, int op_s, int op_tag) {
  cache->line[op_s][i].valid = 1;
  cache->line[op_s][i].tag = op_tag;
  for (int k = 0; k < cache->E; k++)
    if (cache->line[op_s][k].valid == 1) cache->line[op_s][k].time_tamp++;
  cache->line[op_s][i].time_tamp = 0;
}

void UpdateInfo(int op_tag, int op_s) {
  int index = GetIndex(op_s, op_tag);
  if (index == -1) {
    miss_count++;
    if (print_detail) printf("miss\n");
    int i = IsFull(op_s);
    if (i == -1) {
      eviction_count++;
      if (print_detail) printf("eviction\n");
      i = FindLRU(op_s);
    }
    Update(i, op_s, op_tag);
  } else {
    hit_count++;
    if (print_detail) printf("hit\n");
    Update(index, op_s, op_tag);
  }
}

void GetTrace(int s, int E, int b) {
  FILE *pFile;
  pFile = fopen(t, "r");
  if (pFile == NULL) {
    exit(-1);
  }
  char identifier;
  unsigned address;
  int size;
  // Reading lines like " M 20,1" or "L 19,3"
  while (fscanf(pFile, " %c %x,%d", &identifier, &address, &size) > 0) {
    // 想办法先得到标记位和组序号
    int op_tag = address >> (s + b);
    int op_s = (address >> b) & ((unsigned)(-1) >> (8 * sizeof(unsigned) - s));
    switch (identifier) {
      case 'M':  // 一次存储一次加载
        UpdateInfo(op_tag, op_s);
        UpdateInfo(op_tag, op_s);
        break;
      case 'L':
        UpdateInfo(op_tag, op_s);
        break;
      case 'S':
        UpdateInfo(op_tag, op_s);
        break;
    }
  }
  fclose(pFile);
}

void PrintHelp() {
  printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
  printf("Options:\n");
  printf("-h         Print this help message.\n");
  printf("-v         Optional print_detail flag.\n");
  printf("-s <num>   Number of set index bits.\n");
  printf("-E <num>   Number of lines per set.\n");
  printf("-b <num>   Number of block offset bits.\n");
  printf("-t <file>  Trace file.\n\n\n");
  printf("Examples:\n");
  printf("linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");
  printf("linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

int main(int argc, char *argv[]) {
  char opt;
  int s, E, b;
  /*
   * s:S=2^s是组的个数
   * E:每组中有多少行
   * b:B=2^b每个缓冲块的字节数
   */
  while (EOF != (opt = getopt(argc, argv, "hvs:E:b:t:"))) {
    switch (opt) {
      case 'h':
        PrintHelp();
        exit(0);
      case 'v':
        print_detail = 1;
        break;
      case 's':
        s = atoi(optarg);
        break;
      case 'E':
        E = atoi(optarg);
        break;
      case 'b':
        b = atoi(optarg);
        break;
      case 't':
        strcpy(t, optarg);
        break;
      default:
        PrintHelp();
        exit(-1);
    }
  }
  InitCache(s, E, b);  // 初始化一个cache
  GetTrace(s, E, b);
  FreeCache();
  printSummary(hit_count, miss_count, eviction_count);
  return 0;
}