This is an x86-64 bomb for self-study students. 

## 准备工作

```shell
objdump -d ./bomb >> bomb.s
gdb bomb
```

## phase_1

```asm
0000000000400ee0 <phase_1>:
  400ee0:       48 83 ec 08             sub    $0x8,%rsp
  400ee4:       be 00 24 40 00          mov    $0x402400,%esi
  400ee9:       e8 4a 04 00 00          callq  401338 <strings_not_equal>
  400eee:       85 c0                   test   %eax,%eax
  400ef0:       74 05                   je     400ef7 <phase_1+0x17>
  400ef2:       e8 43 05 00 00          callq  40143a <explode_bomb>
  400ef7:       48 83 c4 08             add    $0x8,%rsp
  400efb:       c3                      retq
```

这里把0x402400加载进%esi作为函数参数，然后调用了string_not_equal函数进行判断。
紧接着调用test指令，如果eax为0也就是两个字符串相等就跳转到函数结尾，否则调用explode_bomb函数。

```gdb
(gdb) b  phase_1              # 打断点
(gdb) run                     # 执行到下一个断点
(gdb) info r                  # 查看寄存器值
(gdb) print (char*)0x402400   # 查看内存中字符串
$1 = 0x402400 "Border relations with Canada have never been better."
```

得到答案：`Border relations with Canada have never been better.`

## phase_2

```asm
0000000000400efc <phase_2>:
  400efc: 55                    push   %rbp
  400efd: 53                    push   %rbx
  400efe: 48 83 ec 28           sub    $0x28,%rsp
  400f02: 48 89 e6              mov    %rsp,%rsi
  400f05: e8 52 05 00 00        callq  40145c <read_six_numbers>    # 读入六个数，第一个存在($rsp)处
  400f0a: 83 3c 24 01           cmpl   $0x1,(%rsp)                  # 第一个数和1比较
  400f0e: 74 20                 je     400f30 <phase_2+0x34>        # 等于１跳转
  400f10: e8 25 05 00 00        callq  40143a <explode_bomb>        # 否则爆炸
  400f15: eb 19                 jmp    400f30 <phase_2+0x34>
  400f17: 8b 43 fc              mov    -0x4(%rbx),%eax              # 取出rbx-4处的值赋给eax
  400f1a: 01 c0                 add    %eax,%eax                    # eax = eax *2
  400f1c: 39 03                 cmp    %eax,(%rbx)                  # 比较eax*2和rbx处的值,注意:eax是ebx-4处的值，即将rbx和前一个数的两倍比较
  400f1e: 74 05                 je     400f25 <phase_2+0x29>        # 如果相等就跳转，而跳转处的代码是将rbx+4
  400f20: e8 15 05 00 00        callq  40143a <explode_bomb>        # 否则爆炸
  400f25: 48 83 c3 04           add    $0x4,%rbx                    # 将rbx+4
  400f29: 48 39 eb              cmp    %rbp,%rbx                    # 将加4后的值和rbp比较，注意rbp是rsp+24
  # 而rsp是第一个数，一个数四个字节。那么rbp就应该是后那个数后面那个地址，即rbp是个循环哨兵
  400f2c: 75 e9                 jne    400f17 <phase_2+0x1b>        # 不等就继续跳转去循环
  400f2e: eb 0c                 jmp    400f3c <phase_2+0x40>        # 相等就结束跳转到函数结尾
  400f30: 48 8d 5c 24 04        lea    0x4(%rsp),%rbx               # 将rsp+4存到rbx
  400f35: 48 8d 6c 24 18        lea    0x18(%rsp),%rbp              # 将rsp +24 存到rbp
  400f3a: eb db                 jmp    400f17 <phase_2+0x1b>        # 跳转
  400f3c: 48 83 c4 28           add    $0x28,%rsp
  400f40: 5b                    pop    %rbx
  400f41: 5d                    pop    %rbp
  400f42: c3                    retq   
```

先用`sub $0x28, %rsp`分配了一个 40 bytes 的栈空间，然后调用函数read_six_numbers。

定位到read_six_numbers，其中有一行callq 400bf0 <__isoc99_sscanf@plt>调用库函数sscanf。打印出$0x4025c3地址的字符串：

```gdb
(gdb) print (char*)0x4025c3
$2 = 0x4025c3 "%d %d %d %d %d %d"
```

可以确定，phase_2调用了read_six_numbers来读取六个数字。因此，只需要找出这六个数，就可以拆除炸弹了。

继续分析phase_2:

```gdb
400f0a:   83 3c 24 01             cmpl   $0x1,(%rsp)
400f0e:   74 20                   je     400f30 <phase_2+0x34>
400f10:   e8 25 05 00 00          callq  40143a <explode_bomb>
```

这里比较了处于栈顶的数和$0x1，如果不相等就会调用explode_bomb函数，所以确定第一个数就是1。

再根据je跳转，发现这里将%rsp+0x4和%rsp+0x18对应的值存入了寄存器，然后跳转到400f17，而这里将%rbx-4指向的数放入%eax后，对其乘以2再和%rbx对应的数进行比较，如果不相等就会引爆炸弹，否则就是一个循环。

至此，我们可以确定这里是一个循环，并且每次都是将%rbx-4对应的值乘以2和%rbx对应的值进行比较，也就是说，每次都是将栈中前一个数的两倍和后一个数比较。因此，可以确定这六个数是以1开头倍增的数列。

得到答案: `1 2 4 8 16 32`


## reference

1. [CSAPP Lab -- Bomb Lab](https://zhuanlan.zhihu.com/p/36614408)
2. [CSAPP: Bomb Lab 详细实验解析](https://juejin.cn/post/6874568541229334541)