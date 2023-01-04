This is an x86-64 bomb for self-study students. 

## 准备工作

题目提供了`bomb`和`bomb.c`两个文件，分别是已完成编译的可执行文件，以及展示了主函数大致结构的c文件。

我们需要提供一个字符串文件，以实现对六个炸弹进行拆弹。

我们可以使用的工具分别是反汇编和gdb调试。

```shell
objdump -d ./bomb >> bomb.s
gdb bomb
```

## phase_1

### asm code

```s
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

### analyze

这里把0x402400加载进%esi作为函数参数，然后调用了string_not_equal函数进行判断。
紧接着调用test指令，如果eax为0也就是两个字符串相等就跳转到函数结尾，否则调用explode_bomb函数。

```gdb
(gdb) b  phase_1              # 打断点
(gdb) run                     # 执行到下一个断点
(gdb) info r                  # 查看寄存器值
(gdb) print (char*)0x402400   # 查看内存中字符串 等效于 x/s 0x402400
$1 = 0x402400 "Border relations with Canada have never been better."
```

得到答案：`Border relations with Canada have never been better.`

### c-like code

```c
// 汇编 to C
phase_1(rdi) {
	esi = 0x402400;
	string_no_equal(rdi, esi);
	if (eax == 0)
		callq explode_bomb;
	retq
}

// 整理
void phase_1(char* output) {
  if( string_not_equal(output, "Border relations with Canada have never been better.") == 0) 
    explode_bomb();
  return;
}

```



## phase_2

### asm code

```s
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

### analyze

先用`sub $0x28, %rsp`分配了一个 40 bytes 的栈空间，然后调用函数read_six_numbers。

定位到read_six_numbers，其中有一行callq 400bf0 <__isoc99_sscanf@plt>调用库函数sscanf。打印出$0x4025c3地址的字符串：

```gdb
(gdb) print (char*)0x4025c3
$2 = 0x4025c3 "%d %d %d %d %d %d"
```

可以确定，phase_2调用了read_six_numbers来读取六个数字。因此，只需要找出这六个数，就可以拆除炸弹了。

继续分析phase_2:

```s
400f0a:   83 3c 24 01             cmpl   $0x1,(%rsp)
400f0e:   74 20                   je     400f30 <phase_2+0x34>
400f10:   e8 25 05 00 00          callq  40143a <explode_bomb>
```

这里比较了处于栈顶的数和$0x1，如果不相等就会调用explode_bomb函数，所以确定第一个数就是1。

再根据je跳转，发现这里将%rsp+0x4和%rsp+0x18对应的值存入了寄存器，然后跳转到400f17，而这里将%rbx-4指向的数放入%eax后，对其乘以2再和%rbx对应的数进行比较，如果不相等就会引爆炸弹，否则就是一个循环。

至此，我们可以确定这里是一个循环，并且每次都是将%rbx-4对应的值乘以2和%rbx对应的值进行比较，也就是说，每次都是将栈中前一个数的两倍和后一个数比较。因此，可以确定这六个数是以1开头倍增的数列。

得到答案: `1 2 4 8 16 32`

### c-like code

```c
// 汇编 to C
phase_2(rdi){
	rsi = rsp;
	callq read_six_number;
	if (*rsp == 1)
		goto 400f30;
	else 
		callq explode_bomb;
	goto 400f30;
400f17:
	eax = *(rbx-4);
	eax += eax;
	if (eax == *rbx)
		goto 400f25;
	else 
		callq explode_bomb;
400f25:
	rbx += 4;			# 下一个元素
	if (rbx != rbp)
		goto 400f17;	# 循环
	else 
		goto 400f3c;
400f30:
	rbx = rsp + 4;		# 初始化 init
	rbp = rsp + 24;		# 数组结束
	goto 400f17;
400f3c:
	retq
}

// 整理
phase_2(edi){
	rsi = rsp;
	read_six_number(rdi, rsi);
	if (*rsp != 1)
		explode_bomb();
	rbx = rsp + 4;
	rbp = rsp + 24;
	do {
		eax = *(rbx-4);
		eax += eax;
		if (eax != *rbx)
			explode_bomb();
		rbx += 4;
	} while(rbx != rbp);
	retq;
}

// 语义化
void phase_2(char* output){
  int array[6];
  read_six_numbers(output, array);
  if (array[0] != 1) explode_bomb();
  for (int i = 1; i < 6; i++) {
    array[i-1] += array[i-1];
    if (array[i] != array[i-1]) explode_bomb();
  }
  return ;
}
```


## phase_3

### asm code

```s
0000000000400f43 <phase_3>:
  400f43:	48 83 ec 18          	sub    $0x18,%rsp
  400f47:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx         # 将rsp+0xc的值分别给rcx
  400f4c:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx         # 将rsp+0x8的值给与rdx
  400f51:	be cf 25 40 00       	mov    $0x4025cf,%esi         # 将地址$0x4025cf给esi
  400f56:	b8 00 00 00 00       	mov    $0x0,%eax              # 将eax置为0
  400f5b:	e8 90 fc ff ff       	callq  400bf0 <__isoc99_sscanf@plt> # 调用了库函数sscanf
  400f60:	83 f8 01             	cmp    $0x1,%eax              # sscanf 的返回值是成功读入的数值个数
  # 将成功读入的个数与1进行比较，如果大于1则跳过引爆的代码
  400f63:	7f 05                	jg     400f6a <phase_3+0x27>  
  400f65:	e8 d0 04 00 00       	callq  40143a <explode_bomb>
  400f6a:	83 7c 24 08 07       	cmpl   $0x7,0x8(%rsp)         # compare with 7
  400f6f:	77 3c                	ja     400fad <phase_3+0x6a>  # 如果大于则引爆
  400f71:	8b 44 24 08          	mov    0x8(%rsp),%eax         # 将rsp+0x8中存放的值复制入eax
  400f75:	ff 24 c5 70 24 40 00 	jmpq   *0x402470(,%rax,8)
  400f7c:	b8 cf 00 00 00       	mov    $0xcf,%eax             # 0
  400f81:	eb 3b                	jmp    400fbe <phase_3+0x7b>
  400f83:	b8 c3 02 00 00       	mov    $0x2c3,%eax            # 2
  400f88:	eb 34                	jmp    400fbe <phase_3+0x7b>
  400f8a:	b8 00 01 00 00       	mov    $0x100,%               # 3
  400f8f:	eb 2d                	jmp    400fbe <phase_3+0x7b>
  400f91:	b8 85 01 00 00       	mov    $0x185,%eax            # 4
  400f96:	eb 26                	jmp    400fbe <phase_3+0x7b>
  400f98:	b8 ce 00 00 00       	mov    $0xce,%eax             # 5
  400f9d:	eb 1f                	jmp    400fbe <phase_3+0x7b>
  400f9f:	b8 aa 02 00 00       	mov    $0x2aa,%eax            # 6
  400fa4:	eb 18                	jmp    400fbe <phase_3+0x7b>
  400fa6:	b8 47 01 00 00       	mov    $0x147,%eax            # 7
  400fab:	eb 11                	jmp    400fbe <phase_3+0x7b>
  400fad:	e8 88 04 00 00       	callq  40143a <explode_bomb>
  400fb2:	b8 00 00 00 00       	mov    $0x0,%eax
  400fb7:	eb 05                	jmp    400fbe <phase_3+0x7b>
  400fb9:	b8 37 01 00 00       	mov    $0x137,%eax            # 1
  400fbe:	3b 44 24 0c          	cmp    0xc(%rsp),%eax
  400fc2:	74 05                	je     400fc9 <phase_3+0x86>
  400fc4:	e8 71 04 00 00       	callq  40143a <explode_bomb>
  400fc9:	48 83 c4 18          	add    $0x18,%rsp
  400fcd:	c3                   	retq   
```

### analyze

开始调用了`sscanf`，和phase2中一样，根据`mov $0x4025cf,%esi`可判断`sscanf`函数参数：

```gdb
(gdb) print (char*)0x4025cf
$1 = 0x4025cf "%d %d"
```

可以得知我们输入的应该是两个整数。

先是`%rax`只有大于1，才不会爆炸。然后比较`(%rsp+8)`对应的值和`0x7`的大小关系，如果大于7就会爆炸，然后`jmpq *0x402470(,%rax,8)`进行跳转。

这样的结构是switch的特征，再结合后面的若干`mov`和`jmp`，可以断定这是一个switch索引结构。

每一个jmp可以看做是一个case语句，每一个case语句我们看到都是在将一个参数赋值给`eax`,比如0xcf、0x2c3等，然后所有case统一跳转到`0x400fbe`。

`0x400fbe` 为 `cmp 0xc(%rsp),%eax`，这是将我们输入的第二个数和eax中的值比较，相等就跳过炸弹否则爆炸。

所以，我们推断，输入的两个数中的第一个数应该是索引，然后第二个数就是索引对应的case中的数。

switch索引结构中的内容可以通过gdb查看。

```gdb
(gdb) x/16a 0x402470
0x402470:       0x400f7c <phase_3+57>   0x400fb9 <phase_3+118>
0x402480:       0x400f83 <phase_3+64>   0x400f8a <phase_3+71>
0x402490:       0x400f91 <phase_3+78>   0x400f98 <phase_3+85>
0x4024a0:       0x400f9f <phase_3+92>   0x400fa6 <phase_3+99>
0x4024b0 <array.3449>:  0x737265697564616d      0x6c796276746f666e
0x4024c0:       0x7420756f79206f53      0x756f79206b6e6968
0x4024d0:       0x6f7473206e616320      0x6f62206568742070
0x4024e0:       0x206874697720626d      0x202c632d6c727463
```

需要注意的是C代码会将switch结构转化为一个跳转表(jump table)，其实质是一个数组，以访问代码位置。

|index | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
|-|-|-|-|-|-|-|-|-|
|address | 0x400f7c | 0x400fb9 | 0x400f83 | 0x400f8a | 0x400f91 | 0x400f98 | 0x400f9f | 0x400fa6|
|%eax | 207 | 311 | 707 | 256 | 389 | 206 | 682 | 327|

故有以下答案：

```
0 127
1 311
2 707
3 256
4 389
5 206
6 682
7 327
```

### c-like code

```c
// 汇编 to C 
phase_3(rdi) {
	# 省略分配栈上空间
	rcx = rsp + 0xc;
	rdx = rsp + 0x8;
	esi = 0x4025cf;
	eax = 0;
	sccanf(rdi, esi, rdx, rcx);
	if (eax <= 1)
		explode_bomb();
	if(*(rsp + 8) > 7) {
		explode_bomb();
	}
	goto *(0x402470+rax*8);
400f7c：
	eax = 0xcf;
	goto 400fbe;
400f83：
	eax = 0x2c3;
	goto 400fbe;
400f8a:
	eax = 0x100;
	goto 400fbe;
400f91:
	eax = 0x185;
	goto 400fbe;
400f98:
	eax = 0xce;
	goto 400fbe;
400f9f:
	eax = 0x2aa;
	goto 400fbe;
400fa6:
	eax = 0x147;
	goto 400fbe;
	eax = 0;
	goto 400fbe;
400fb9:
	eax = 0x137;
400fbe:
	if (eax != *(rsp+0xc))
		explode_bomb();
	retq;
}

// 整理
void phase_3(char* output) {
    int x, y;
    if(sscanf(output, "%d %d", &x, &y) <= 1)
        explode_bomb();
    if(x > 7)
        explode_bomb();
    int num;
    switch(x) {
    case 0:
        num = 207;
    	break;
    case 1:
        num = 311;
        break;
    case 2:
        num = 707;
        break;
    case 3:
        num = 256;
        break;
    case 4:
        num = 389;
        break;
    case 5:
        num = 206;
        break;
    case 6:
        num = 682;
		break;
    case 7:
        num = 327;
    }
    if (num != y)
        explode_bomb();
    return;
}
```


## phase_4

### asm code

```s
000000000040100c <phase_4>:
  40100c: 48 83 ec 18           sub    $0x18,%rsp
  401010: 48 8d 4c 24 0c        lea    0xc(%rsp),%rcx
  401015: 48 8d 54 24 08        lea    0x8(%rsp),%rdx
  40101a: be cf 25 40 00        mov    $0x4025cf,%esi
  40101f: b8 00 00 00 00        mov    $0x0,%eax
  401024: e8 c7 fb ff ff        callq  400bf0 <__isoc99_sscanf@plt>
  401029: 83 f8 02              cmp    $0x2,%eax
  40102c: 75 07                 jne    401035 <phase_4+0x29>
  40102e: 83 7c 24 08 0e        cmpl   $0xe,0x8(%rsp) # 第一个数 <= 0xe
  401033: 76 05                 jbe    40103a <phase_4+0x2e> 
  401035: e8 00 04 00 00        callq  40143a <explode_bomb>
  40103a: ba 0e 00 00 00        mov    $0xe,%edx
  40103f: be 00 00 00 00        mov    $0x0,%esi
  401044: 8b 7c 24 08           mov    0x8(%rsp),%edi
  401048: e8 81 ff ff ff        callq  400fce <func4>
  40104d: 85 c0                 test   %eax,%eax      # 测试返回值是否为0，非0爆炸
  40104f: 75 07                 jne    401058 <phase_4+0x4c>
  401051: 83 7c 24 0c 00        cmpl   $0x0,0xc(%rsp)
  401056: 74 05                 je     40105d <phase_4+0x51>
  401058: e8 dd 03 00 00        callq  40143a <explode_bomb>
  40105d: 48 83 c4 18           add    $0x18,%rsp
  401061: c3                    retq   
```

```s
0000000000400fce <func4>: 
  400fce: sub    $0x8,%rsp                      # 分配栈帧
  400fd2: mov    %edx,%eax                      # C             eax
  400fd4: sub    %esi,%eax                      # C - B         更新 eax
  400fd6: mov    %eax,%ecx                      # C - B         ecx 
  400fd8: shr    $0x1f,%ecx                     # 右移 31 位， ecx 长为 32 位（也就是之前的最高位变为最低位，其余 31 位填充补 0），可以认为 ecx = 0
  400fdb: add    %ecx,%eax                      # C - B         eax
  400fdd: sar    %eax                           # 这里是一个缩写 sar $1,%eax (对应的机器码为 D1F8)  eax = (C-B)/2
  400fdf: lea    (%rax,%rsi,1),%ecx             # (C+B)/2       ecx        
  400fe2: cmp    %edi,%ecx                      # ecx 与 A 进行比较 
  400fe4: jle    400ff2 <func4+0x24>            # ecx 小于等于 A 则跳转
  400fe6: lea    -0x1(%rcx),%edx                # C = (C+B)/2 - 1
  400fe9: callq  400fce <func4>                 # 递归调用
  400fee: add    %eax,%eax                      # 递归返回值加倍
  400ff0: jmp    401007 <func4+0x39>            # 跳转到 func 函数的出口处 
  400ff2: mov    $0x0,%eax                      # eax = 0
  400ff7: cmp    %edi,%ecx                      # ecx 与 A 进行比较
  400ff9: jge    401007 <func4+0x39>            # eax 大于等于 A 则跳转
  400ffb: lea    0x1(%rcx),%esi                 # B = ecx + 1
  400ffe: callq  400fce <func4>                 # 递归调用
  401003: lea    0x1(%rax,%rax,1),%eax          # 递归返回值加倍并再加上 1
  401007: add    $0x8,%rsp                      # 释放栈帧
  40100b: retq                                  # 函数返回
```

### analyze

1. sscanf和上一题的格式字符串一样是"%d %d"。
2. func4内部在调用func4，递归的汇编
3. phase_4的功能：解析字符串为两个数字x、y，x必须小于等于14，调用func4，x、0、14分别为其参数，func4返回值必须为0，y必须为0
4. func4的功能：二分法找出元素x，目标值在数组的前半部分则返回奇数，后半部分则返回偶数；找到元素x返回0；如果目标值在二分查找过程中一直在左半边，则会返回0。

0 1 3 7一直在左半区

有以下答案：
```
0 0
1 0
3 0
7 0
```

### c-like code

```c
// 汇编 to C
phase_4(rdi) {
	# 省略分配栈上空间
	rcx = rsp + 12;			# sscanf 参数 4
	rdx = rsp + 8;			# sscanf 参数 3
	rsi = 0x4025cf;			# sscanf 参数 2
    rax = 0;
	callq sscanf;
	if (rax != 2) {			# sscanf 返回值判断
		goto 401035;
	}
	if (0x8(rsp) <= 14) {	# 第一个数字
		goto 40103a;
	}
401035:
	goto explode_bomb;
40103a:
	edx = 14;			# func4 参数 3
	rsi = 0;			# func4 参数 2
	rdi = 0x8(rsp);		# func4 参数 1
	goto func4;
	if (rax != 0) {		# func4 返回值判断
		goto 401058;
	}
	if (0xc(rsp) == 0) {	# # 第二个数字
		goto 40105d;
	}
401058:
	goto exlode_bomb;
40105d:
	return;
}

// 整理
phase_4(rdi) {
	rcx = rsp + 12;
	rdx = rsp + 8;
	rsi = 0x4025cf;
	rax = 0;
	rax = sscanf(rdi, rsi, rdx, rcx);
	if (rax != 2) {
		explode_bomb();
	}
	if (*(rsp + 8) <= 14) {
    rdx = 14;
    rsi = 0;
    rdi = *(rsp + 8);
    rax = func4(rdi, rsi, rdx);
    if (rax != 0) explode_bomb();
    if (*(rsp + 12) != 0) explode_bomb();
	}
	retq;
}

// 语义化
void phase_4(char* input) {
  int x, y;
  int ret = sscanf(input, "%d %d", &x, &y);
	if (ret != 2 || x > 14) {
		explode_bomb();
	} else {
    int ret = func4(x, 0, 14);
    if (ret != 0 || y != 0) {
        explode_bomb();
    }
	}
	return ;
}

// 汇编 to C
func4(rdi, rsi, rdx) {
	eax = edx;
	eax -= esi;
	ecx = eax;
	ecx >>= 31;
	eax += ecx;
	eax >>= 1;				# 对eax一顿操作
	ecx = rax + rsi;		# 对ecx一顿操作
	if (ecx <= edi) {
		goto 400ff2;
	}
	edx = rcx - 1;
	goto func4;				# 递归
	eax += eax;
	goto 401007;
400ff2:
	eax = 0;
	if (ecx >= edi) {		# 递归基
		goto 401007;
	}
	esi = rcx + 1;
	goto func4;				# 递归
	eax = rax + rax + 1;
401007:
	return;
}


// 整理
int func4(int x, int a, int b) {
  int num = b - a;
  num = (num + num >> 31) / 2;
  int c = num + a;
  if (c <= x) {
  	if (c >= x) return 0;
    return 2 * func4(x, num+1, b) + 1;
  }
  return 2 * func4(x, a, num-1);
}

// 语义化
int func4(int target, int step, int limit) {
  int temp = (limit - step) * 0.5;
  int mid = temp + step;
  if (mid > target) {
    limit = mid - 1;
    int ret1 = func4(target, step, limit);
    return 2 * ret1;
  } else {
    if (mid >= target) {
      return 0;
    } else {
      step = mid + 1;
      int ret2 = func4(target, step, limit);
      return (2 * ret2 + 1);
    }
  }
}
```

## phase_5

### asm code

```s
0000000000401062 <phase_5>:
  401062:	53                   	push   %rbx
  401063:	48 83 ec 20          	sub    $0x20,%rsp
  401067:	48 89 fb             	mov    %rdi,%rbx
  40106a:	64 48 8b 04 25 28 00 	mov    %fs:0x28,%rax               # 金丝雀
  401071:	00 00 
  401073:	48 89 44 24 18       	mov    %rax,0x18(%rsp)
  401078:	31 c0                	xor    %eax,%eax                   # 清零
  40107a:	e8 9c 02 00 00       	callq  40131b <string_length>      # 计算字符串长度
  40107f:	83 f8 06             	cmp    $0x6,%eax
  401082:	74 4e                	je     4010d2 <phase_5+0x70>
  401084:	e8 b1 03 00 00       	callq  40143a <explode_bomb>
  401089:	eb 47                	jmp    4010d2 <phase_5+0x70>
  40108b:	0f b6 0c 03          	movzbl (%rbx,%rax,1),%ecx
  40108f:	88 0c 24             	mov    %cl,(%rsp)
  401092:	48 8b 14 24          	mov    (%rsp),%rdx
  401096:	83 e2 0f             	and    $0xf,%edx                   # 取edx后四位
  401099: 0f b6 92 b0 24 40 00  movzbl 0x4024b0(%rdx),%edx         # 将edx后四位作为0x4024b0字符数组的索引值
  4010a0: 88 54 04 10           mov    %dl,0x10(%rsp,%rax,1)       # 依次拷贝字符数组到0x10((%rsp,%rax,1))
  4010a4: 48 83 c0 01           add    $0x1,%rax                   # 循环计数+1
  4010a8: 48 83 f8 06           cmp    $0x6,%rax                   # 循环计数和6比较，即循环6次
  4010ac: 75 dd                 jne    40108b <phase_5+0x29>
  4010ae: c6 44 24 16 00        movb   $0x0,0x16(%rsp)             # 字符串末尾添加"\0"
  4010b3: be 5e 24 40 00        mov    $0x40245e,%esi              # 字符串常量
  4010b8: 48 8d 7c 24 10        lea    0x10(%rsp),%rdi
  4010bd: e8 76 02 00 00        callq  401338 <strings_not_equal>  # 和字符串常量比较
  4010c2:	85 c0                	test   %eax,%eax
  4010c4:	74 13                	je     4010d9 <phase_5+0x77>
  4010c6:	e8 6f 03 00 00       	callq  40143a <explode_bomb>
  4010cb:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
  4010d0:	eb 07                	jmp    4010d9 <phase_5+0x77>
  4010d2:	b8 00 00 00 00       	mov    $0x0,%eax
  4010d7:	eb b2                	jmp    40108b <phase_5+0x29>
  4010d9:	48 8b 44 24 18       	mov    0x18(%rsp),%rax
  4010de:	64 48 33 04 25 28 00 	xor    %fs:0x28,%rax
  4010e5:	00 00 
  4010e7:	74 05                	je     4010ee <phase_5+0x8c>
  4010e9:	e8 42 fa ff ff       	callq  400b30 <__stack_chk_fail@plt> # 栈被破坏
  4010ee:	48 83 c4 20          	add    $0x20,%rsp
  4010f2:	5b                   	pop    %rbx
  4010f3:	c3                   	retq   
```

### analyze

查看`4010bd`处地址为`0x4024b0`的字符串常量。

```gdb
(gdb) x/s 0x4024b0
0x4024b0 <array.3449>:	"maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?"
```

取output字符串的字符，然后逐次将每个字符与0xf与操作，得到的值做为`0x4024b0`处字符串的下标。

与0xf与操作意味着能取到`0x4024b0`处字符串的范围是0-15。

可以得到前16为字符串：maduiersnfotvbyl。

通过for循环能够生成一个字符串，该字符串由output的 每个字符&0xf 来作为maduiersnfotvbyl的下标，来选择字符。

然后*(rsp + 0x16) = 0是字符串结尾符，最后将该字符串与0x40245e处的字符串进行比较。

0x40245e处的字符串：

```gdb
(gdb) x/s 0x40245e
0x40245e:	"flyers"
```

该函数功能如下：

1. output长度必须等于6
2. 生成一个新的字符串str，由output每个字符&0xf作为maduiersnfotvbyl下标得到
3. str必须为flyers

我们输入的每个字符，都会取最低4位，然后转换成数字来作为长字符串 "maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?"的索引，最后依次取出的字符就是"flyers"。

所以，找出"flyers"在长字符串中的索引分别是:9, 15, 14, 5, 6, 7。为了凑成字符，对每个加上64恰好都是大写字母的ascii码:73, 79, 78, 69, 70, 71。

通过枚举ascii letters，可以找到六个&0xf分别为 9 15 14 5 6 7的字符：

```
9 : i y I Y
15: o O
14: n N
5 : e u E U
6 : f v F V
7 : g w G W
```

其他合法字符亦可，任意组合即可

### c-like code

```c
// 汇编 to C
phase_5(rdi) {
  rbx = rdi;
  eax = eax ^ eax;
  eax = string_length(rdi);
  if (eax == 6) {
    goto 4010d2;
  } else
    explode_bomb();
  for (eax = 0; eax != 6; eax++) {
    ecx = rbx + rax;
    *rsp = cl;
    rdx = *rsp;
    edx = edx & 0xf;
    edx = *(rdx + 0x4024b0);
    *(rsp + rax + 0x10) = dl;
  }
  *(rsp + 0x16) = 0;
  esi = 0x40245e;
  rdi = rsp + 10;
  strings_not_equal(rdi, rsi);
  if (eax != 0) {
    explode_bomb();
  }
}

// 整理
const char g_str[16] = "maduiersnfotvbyl";
void phase_5(char* output) {
  char str[7];
  if (string_length(output) != 6) {
    explode_bomb();
  }
  for (int i = 0; i != 6; i++) {
    str[i] = g_str[output[i] & 0xf];
  }
  str[7] = '\0';
  if (string_not_equal(str, "flyers") != 0) {
    explode_bomb();
  }
}
```

## phase_6

### asm code

```s
00000000004010f4 <phase_6>:
  4010f4:	41 56                	push   %r14
  4010f6:	41 55                	push   %r13
  4010f8:	41 54                	push   %r12
  4010fa:	55                   	push   %rbp
  4010fb:	53                   	push   %rbx
  4010fc:	48 83 ec 50          	sub    $0x50,%rsp
  401100:	49 89 e5             	mov    %rsp,%r13
  401103:	48 89 e6             	mov    %rsp,%rsi
  401106:	e8 51 03 00 00       	callq  40145c <read_six_numbers>
  40110b:	49 89 e6             	mov    %rsp,%r14
  40110e:	41 bc 00 00 00 00    	mov    $0x0,%r12d # i = 0
  401114:	4c 89 ed             	mov    %r13,%rbp
  401117:	41 8b 45 00          	mov    0x0(%r13),%eax
  40111b:	83 e8 01             	sub    $0x1,%eax
  40111e:	83 f8 05             	cmp    $0x5,%eax
  401121:	76 05                	jbe    401128 <phase_6+0x34>
  401123:	e8 12 03 00 00       	callq  40143a <explode_bomb>
  401128:	41 83 c4 01          	add    $0x1,%r12d # ++i
  40112c:	41 83 fc 06          	cmp    $0x6,%r12d # == 6 外层break
  401130:	74 21                	je     401153 <phase_6+0x5f>
  401132:	44 89 e3             	mov    %r12d,%ebx
  401135:	48 63 c3             	movslq %ebx,%rax # i = j
  401138:	8b 04 84             	mov    (%rsp,%rax,4),%eax
  40113b:	39 45 00             	cmp    %eax,0x0(%rbp)
  40113e:	75 05                	jne    401145 <phase_6+0x51>
  401140:	e8 f5 02 00 00       	callq  40143a <explode_bomb>
  401145:	83 c3 01             	add    $0x1,%ebx  # ++j
  401148:	83 fb 05             	cmp    $0x5,%ebx  # <=5 内层break
  40114b:	7e e8                	jle    401135 <phase_6+0x41>
  40114d:	49 83 c5 04          	add    $0x4,%r13
  401151:	eb c1                	jmp    401114 <phase_6+0x20>
  401153:	48 8d 74 24 18       	lea    0x18(%rsp),%rsi
  401158:	4c 89 f0             	mov    %r14,%rax
  40115b:	b9 07 00 00 00       	mov    $0x7,%ecx
  401160:	89 ca                	mov    %ecx,%edx
  401162:	2b 10                	sub    (%rax),%edx
  401164:	89 10                	mov    %edx,(%rax)
  401166:	48 83 c0 04          	add    $0x4,%rax
  40116a:	48 39 f0             	cmp    %rsi,%rax
  40116d:	75 f1                	jne    401160 <phase_6+0x6c>
  40116f:	be 00 00 00 00       	mov    $0x0,%esi
  401174:	eb 21                	jmp    401197 <phase_6+0xa3>
  401176:	48 8b 52 08          	mov    0x8(%rdx),%rdx
  40117a:	83 c0 01             	add    $0x1,%eax
  40117d:	39 c8                	cmp    %ecx,%eax
  40117f:	75 f5                	jne    401176 <phase_6+0x82>
  401181:	eb 05                	jmp    401188 <phase_6+0x94>
  401183:	ba d0 32 60 00       	mov    $0x6032d0,%edx
  401188:	48 89 54 74 20       	mov    %rdx,0x20(%rsp,%rsi,2)
  40118d:	48 83 c6 04          	add    $0x4,%rsi
  401191:	48 83 fe 18          	cmp    $0x18,%rsi
  401195:	74 14                	je     4011ab <phase_6+0xb7>
  401197:	8b 0c 34             	mov    (%rsp,%rsi,1),%ecx
  40119a:	83 f9 01             	cmp    $0x1,%ecx
  40119d:	7e e4                	jle    401183 <phase_6+0x8f>
  40119f:	b8 01 00 00 00       	mov    $0x1,%eax
  4011a4:	ba d0 32 60 00       	mov    $0x6032d0,%edx
  4011a9:	eb cb                	jmp    401176 <phase_6+0x82>
  4011ab:	48 8b 5c 24 20       	mov    0x20(%rsp),%rbx
  4011b0:	48 8d 44 24 28       	lea    0x28(%rsp),%rax
  4011b5:	48 8d 74 24 50       	lea    0x50(%rsp),%rsi
  4011ba:	48 89 d9             	mov    %rbx,%rcx
  4011bd:	48 8b 10             	mov    (%rax),%rdx
  4011c0:	48 89 51 08          	mov    %rdx,0x8(%rcx)
  4011c4:	48 83 c0 08          	add    $0x8,%rax
  4011c8:	48 39 f0             	cmp    %rsi,%rax
  4011cb:	74 05                	je     4011d2 <phase_6+0xde>
  4011cd:	48 89 d1             	mov    %rdx,%rcx
  4011d0:	eb eb                	jmp    4011bd <phase_6+0xc9>
  4011d2:	48 c7 42 08 00 00 00 	movq   $0x0,0x8(%rdx)
  4011d9:	00 
  4011da:	bd 05 00 00 00       	mov    $0x5,%ebp
  4011df:	48 8b 43 08          	mov    0x8(%rbx),%rax
  4011e3:	8b 00                	mov    (%rax),%eax
  4011e5:	39 03                	cmp    %eax,(%rbx)
  4011e7:	7d 05                	jge    4011ee <phase_6+0xfa>
  4011e9:	e8 4c 02 00 00       	callq  40143a <explode_bomb>
  4011ee:	48 8b 5b 08          	mov    0x8(%rbx),%rbx
  4011f2:	83 ed 01             	sub    $0x1,%ebp
  4011f5:	75 e8                	jne    4011df <phase_6+0xeb>
  4011f7:	48 83 c4 50          	add    $0x50,%rsp
  4011fb:	5b                   	pop    %rbx
  4011fc:	5d                   	pop    %rbp
  4011fd:	41 5c                	pop    %r12
  4011ff:	41 5d                	pop    %r13
  401201:	41 5e                	pop    %r14
  401203:	c3                   	retq   
  ```

### analyze

1. 需要输入6个数字
2. 第一个循环对这个整数数组中的值是否相同做了嵌套循环的判断，如果相同，就会触发炸弹
3. 第二个循环修改了整数数组的值：`for i = 0 -> 5 :  array[i] = 7 - array[i]`；
4. 第三个循环根据可以嵌套解指针的结构（链表）构造了一个指针数组：根据array的值，取到对应位置的listnode，并由ptr_array存储；
5. 第四个循环对指针数组做了修改：链表重排；
6. 第五个循环对嵌套解指针的结构（链表）进行是否为降序判断。

该链表的地址和值可以通过gdb查出：

```
0x6032d0 -> 0x6032e0 -> 0x6032f0 -> 0x603300 -> 0x603310 -> 0x603320
332         168         924         691         477         443
```

这里有一个细节，第一个循环中将 `eax -= 1`，然后和5比较通过 `jbe` 跳转，目的是防止 `eax` 取负数和0，`eax -= 1`是防止取0，`jbe`防止取负数。

因为`jbe`跳转是用无符号数比较，负数在无符号数编码下是很大的数所以必然大于5，因此输入的数字范围是1-6。
  
因此`phase_6`就是链表排序，输入的数字为原链表位置，输入的次序为新链表的位置。

又因为第二个循环对`array`中的每个数字 `7 - array[i]`，因此输入的数字需要将链表重排成为升序。

原链表的从小到大顺序是 5 6 1 2 3 4， 重排后 4 3 2 1 6 5。

答案为`4 3 2 1 6 5`

### c-like code

```c
phase_6(rdi) {
  r13 = rsp;
  rsi = rsp;
  read_six_numbers(rdi, rsi);
  r14 = rsp;
  for (r12 = 0; r12 != 6; r12++) {
    rbp = r13;
    eax = *r13;
    eax -= 1;
    if (eax > 5) explode_bomb();
    for (ebx = r12 + 1; ebx <= 5; ebx++) {
      rax = ebx;
      eax = *(rsp + rax * 4);
      if (*rbp == eax) explode_bomb();
    }
    r13 += 4;
  }

  rsi = rsp + 0x18;
  rax = r14;
  ecx = 7;
  for (rax = r14; rax != rsi; rax += 4) {
    edx = ecx;
    eax -= *rax;
    *rax = edx;
  }

  for (rsi = 0; rsi != 0x18; rsi += 4) {
    ecx = *(rsp + rsi);
    if (ecx <= 1)
      edx = 0x6032d0;
    else {
      edx = 0x6032d0;
      for (eax = 1; eax != ecx; eax++) {
        rdx = *(rdx + 8);
      }
    }
    *(rsp + 2 * rsi + 20) = rdx;
  }

  rbx = *(rsp + 0x20);
  rcx = rbx;
  for (rax = rsp + 0x28; rax != rsp + 0x50; rax += 8) {
    rdx = *rax;
    *(rcx + 8) = rdx;
    rcx = rdx;
  }

  *(rdx + 8) = 0;
  for (ebp = 5; ebp != 0; ebp -= 1) {
    rax = *(rbx + 0x8);
    eax = *rax;
    if (eax < *(rbx)) explode_bomb();
    rbx = *(rbx + 8);
  }

// 整理
typedef struct {
  int val;
  ListNode* next;
} ListNode;

void phase_6(char* output) {
  int array[6];
  ListNode* node_array[6];
  read_six_numbers(output, array);
  // 数字范围必须为1-6且互不重复
  for (int i = 0; i != 6; i++) {
    int num = array[i];
    num--;
    if ((unsigned int)num > 5)  // 最大为6
      explode_bomb();
    for (int j = i + 1; j <= 5; j++) {
      if (array[i] == array[j])  // 每个元素都不重复
        explode_bomb();
    }
  }
  // 修改 array
  for (int i = 0; i < 6; i++) {
    array[i] = (7 - array[i]);
  }
  // 生成 node_array
  for (int i = 0; i < 6; i++) {
    int cur = array[i];
    ListNode* node = 0x6032d0;  // 链表head
    if (cur > 1) {
      for (int j = 1; j < cur; j++) {
        node = node->next;
      }
    }
    node_array[i] = node;
  }
  for (int i = 0; i < 5; i++) {
    node_array[i]->next = node_array[i + 1];
  }
  // 0x6032d0 0x6032e0 0x6032f0 0x603300 0x603310 0x603320
  // 332      168      924      691      477      443
  // 6        5        4        3        2        1
  ListNode* ptr = node_array[0];
  for (int i = 5; i > 0; i--) {
    if (ptr->val < ptr->next->val) explode_bomb();
    ptr = ptr->next;
  }
}

```

## secret_phase

隐藏关在`phase_defused`函数被调用，而`phase_defused`在每次通过一个phase时都会被执行

### asm code

```s
00000000004015c4 <phase_defused>:
  4015c4: 48 83 ec 78           sub    $0x78,%rsp
  4015c8: 64 48 8b 04 25 28 00  mov    %fs:0x28,%rax
  4015cf: 00 00 
  4015d1: 48 89 44 24 68        mov    %rax,0x68(%rsp)
  4015d6: 31 c0                 xor    %eax,%eax                  # 比较输入的字符串数目是否等于6，不等于则跳转至程序结束
  4015d8: 83 3d 81 21 20 00 06  cmpl   $0x6,0x202181(%rip)        # 603760 <num_input_strings>
  4015df: 75 5e                 jne    40163f <phase_defused+0x7b>
  4015e1: 4c 8d 44 24 10        lea    0x10(%rsp),%r8
  4015e6: 48 8d 4c 24 0c        lea    0xc(%rsp),%rcx
  4015eb: 48 8d 54 24 08        lea    0x8(%rsp),%rdx
  4015f0: be 19 26 40 00        mov    $0x402619,%esi             # phase_4 答案
  4015f5: bf 70 38 60 00        mov    $0x603870,%edi             # "%d %d %s"
  4015fa: e8 f1 f5 ff ff        callq  400bf0 <__isoc99_sscanf@plt>
  4015ff: 83 f8 03              cmp    $0x3,%eax                  # 判断参数是否为3
  401602: 75 31                 jne    401635 <phase_defused+0x71>
  401604: be 22 26 40 00        mov    $0x402622,%esi             # “DrEvil”
  401609: 48 8d 7c 24 10        lea    0x10(%rsp),%rdi
  40160e: e8 25 fd ff ff        callq  401338 <strings_not_equal> # 字符串比较
  401613: 85 c0                 test   %eax,%eax
  401615: 75 1e                 jne    401635 <phase_defused+0x71>
  401617: bf f8 24 40 00        mov    $0x4024f8,%edi
  40161c: e8 ef f4 ff ff        callq  400b10 <puts@plt>
  401621: bf 20 25 40 00        mov    $0x402520,%edi
  401626: e8 e5 f4 ff ff        callq  400b10 <puts@plt>
  40162b: b8 00 00 00 00        mov    $0x0,%eax
  401630: e8 0d fc ff ff        callq  401242 <secret_phase>     # 调用secret_phase
  401635: bf 58 25 40 00        mov    $0x402558,%edi            # 顺利通过前六个phase提示语
  40163a: e8 d1 f4 ff ff        callq  400b10 <puts@plt>
  40163f: 48 8b 44 24 68        mov    0x68(%rsp),%rax
  401644: 64 48 33 04 25 28 00  xor    %fs:0x28,%rax
  40164b: 00 00 
  40164d: 74 05                 je     401654 <phase_defused+0x90>
  40164f: e8 dc f4 ff ff        callq  400b30 <__stack_chk_fail@plt>
  401654: 48 83 c4 78           add    $0x78,%rsp
  401658: c3                    retq   
```

```s
0000000000401242 <secret_phase>:
  401242: 53                    push   %rbx
  401243: e8 56 02 00 00        callq  40149e <read_line>
  401248: ba 0a 00 00 00        mov    $0xa,%edx
  40124d: be 00 00 00 00        mov    $0x0,%esi
  401252: 48 89 c7              mov    %rax,%rdi
  401255: e8 76 f9 ff ff        callq  400bd0 <strtol@plt>           # str to long
  40125a: 48 89 c3              mov    %rax,%rbx                     # 将rax保存到rbx中 
  40125d: 8d 40 ff              lea    -0x1(%rax),%eax               # eax = eax -1
  401260: 3d e8 03 00 00        cmp    $0x3e8,%eax                   # cmp 1000, eax
  401265: 76 05                 jbe    40126c <secret_phase+0x2a>    # if  eax <= 1000 then 跳过炸弹
  401267: e8 ce 01 00 00        callq  40143a <explode_bomb>         # 炸弹
  40126c: 89 de                 mov    %ebx,%esi                     # 传参
  40126e: bf f0 30 60 00        mov    $0x6030f0,%edi                # 传参 
  401273: e8 8c ff ff ff        callq  401204 <fun7>                 # 调用fun7
  401278: 83 f8 02              cmp    $0x2,%eax                     # 比较返回值和2
  40127b: 74 05                 je     401282 <secret_phase+0x40>    # 相等就跳转输出0x402438处的字符串并返回
  40127d: e8 b8 01 00 00        callq  40143a <explode_bomb>         # 不等就爆炸
  401282: bf 38 24 40 00        mov    $0x402438,%edi
  401287: e8 84 f8 ff ff        callq  400b10 <puts@plt>
  40128c: e8 33 03 00 00        callq  4015c4 <phase_defused>
  401291: 5b                    pop    %rbx
  401292: c3                    retq   
```

```s
0000000000401204 <fun7>:
  401204: 48 83 ec 08           sub    $0x8,%rsp
  401208: 48 85 ff              test   %rdi,%rdi          # edi如果为0则跳转并返回-1
  40120b: 74 2b                 je     401238 <fun7+0x34>
  40120d: 8b 17                 mov    (%rdi),%edx        # 取出rdi地址的值赋给edx
  40120f: 39 f2                 cmp    %esi,%edx          # 比较edx和esi的值
  401211: 7e 0d                 jle    401220 <fun7+0x1c> # if edx <= esi(这就是strtol转换来的数字)，跳转
  401213: 48 8b 7f 08           mov    0x8(%rdi),%rdi     # 否则执行递归  rdi = (rdi+8)
  401217: e8 e8 ff ff ff        callq  401204 <fun7>      # 递归
  40121c: 01 c0                 add    %eax,%eax          # 递归返回值*2
  40121e: eb 1d                 jmp    40123d <fun7+0x39> # 跳转至返回
  401220: b8 00 00 00 00        mov    $0x0,%eax          # 提前将eax置0,这其实是返回值
  401225: 39 f2                 cmp    %esi,%edx          # 还是比较esi和edx
  401227: 74 14                 je     40123d <fun7+0x39> # 如果相等就跳转并返回0
  401229: 48 8b 7f 10           mov    0x10(%rdi),%rdi    # 如果不相等就 rdi = (rdi+16) 如果传参<36，此处rdi=8 
  40122d: e8 d2 ff ff ff        callq  401204 <fun7>      # 递归2
  401232: 8d 44 00 01           lea    0x1(%rax,%rax,1),%eax # 递归返回值 eax = 2*eax+1
  401236: eb 05                 jmp    40123d <fun7+0x39> # 跳转至返回
  401238: b8 ff ff ff ff        mov    $0xffffffff,%eax
  40123d: 48 83 c4 08           add    $0x8,%rsp
  401241: c3                    retq   
```

### analyze

1. 判断输入是否为6个字符串，也即通过前六关才触发隐藏关检测。
2. 通过gdb调试可知`esi`为"%d %d %s" 而`edi`则是我们phase_4输入的答案，%s无法匹配。
3. 在第四关尾缀字符串`DrEvil`可触发隐藏关
4. `secret_phase`调用`fun7`，`fun7`返回2才能通过 
5. `fun7`中两次递归rdi的变化是不样的，那么为了返回2，递归调用的顺序应为`step3->step2->step1` 也就是`*rdi`的值先要　`*rdi > esi`  ,然后　`*rdi  < esi` ， 最后 `*rdi == esi`。而`esi`是我们输入的，`rdi`在第一次调用`fun7`的时候就是固定的一个数
6. gdb查询`x /128xg 0x6030f0`可以发现这是一个二叉树的结构，每个节点第1个8字节存放数据，第2个8字节存放左子树地址，第3个8字节存放右子树位置。并且命令也有规律，nab，a代表层数，b代表从左至右第b个节点。![](https://upload-images.jianshu.io/upload_images/7004504-57297d89dc11d7d5.png)
7. `fun7`中`edi`指向一个树的节点，令`edi`节点的值与我们读入的值进行比较。如果两者相等：返回0；如果前者大于后者：`rdi`移至左子树，返回`2 * rax`；如果后者大于前者：`rdi`移至右子树，返回`2 * rax + 1`。那么我们需要返回2，应该在最后一次调用返回0，倒数第二次调用返回`2 * rax + 1`，第一次调用返回`2 * rax`。换句话说，这个数应该在第三层，比父节点大且比根结节小。观察上图，唯一的答案是：0x16（22）

```gdb
(gdb) x /128xg 0x6030f0
0x6030f0 <n1>:  0x0000000000000024      0x0000000000603110
0x603100 <n1+16>:       0x0000000000603130      0x0000000000000000
0x603110 <n21>: 0x0000000000000008      0x0000000000603190
0x603120 <n21+16>:      0x0000000000603150      0x0000000000000000
0x603130 <n22>: 0x0000000000000032      0x0000000000603170
0x603140 <n22+16>:      0x00000000006031b0      0x0000000000000000
0x603150 <n32>: 0x0000000000000016      0x0000000000603270
0x603160 <n32+16>:      0x0000000000603230      0x0000000000000000
0x603170 <n33>: 0x000000000000002d      0x00000000006031d0
0x603180 <n33+16>:      0x0000000000603290      0x0000000000000000
0x603190 <n31>: 0x0000000000000006      0x00000000006031f0
0x6031a0 <n31+16>:      0x0000000000603250      0x0000000000000000
0x6031b0 <n34>: 0x000000000000006b      0x0000000000603210
0x6031c0 <n34+16>:      0x00000000006032b0      0x0000000000000000
0x6031d0 <n45>: 0x0000000000000028      0x0000000000000000
0x6031e0 <n45+16>:      0x0000000000000000      0x0000000000000000
0x6031f0 <n41>: 0x0000000000000001      0x0000000000000000
0x603200 <n41+16>:      0x0000000000000000      0x0000000000000000
0x603210 <n47>: 0x0000000000000063      0x0000000000000000
0x603220 <n47+16>:      0x0000000000000000      0x0000000000000000
0x603230 <n44>: 0x0000000000000023      0x0000000000000000
0x603240 <n44+16>:      0x0000000000000000      0x0000000000000000
0x603250 <n42>: 0x0000000000000007      0x0000000000000000
0x603260 <n42+16>:      0x0000000000000000      0x0000000000000000
0x603270 <n43>: 0x0000000000000014      0x0000000000000000
0x603280 <n43+16>:      0x0000000000000000      0x0000000000000000
0x603290 <n46>: 0x000000000000002f      0x0000000000000000
0x6032a0 <n46+16>:      0x0000000000000000      0x0000000000000000
0x6032b0 <n48>: 0x00000000000003e9      0x0000000000000000
0x6032c0 <n48+16>:      0x0000000000000000      0x0000000000000000
0x6032d0 <node1>:       0x000000010000014c      0x00000000006032e0
0x6032e0 <node2>:       0x00000002000000a8      0x00000000006032f0
0x6032f0 <node3>:       0x000000030000039c      0x0000000000603300
0x603300 <node4>:       0x00000004000002b3      0x0000000000603310
0x603310 <node5>:       0x00000005000001dd      0x0000000000603320
0x603320 <node6>:       0x00000006000001bb      0x0000000000000000
0x603330:       0x0000000000000000      0x0000000000000000
0x603340 <host_table>:  0x0000000000402629      0x0000000000402643
0x603350 <host_table+16>:       0x000000000040265d      0x0000000000000000
0x603360 <host_table+32>:       0x0000000000000000      0x0000000000000000
0x603370 <host_table+48>:       0x0000000000000000      0x0000000000000000
0x603380 <host_table+64>:       0x0000000000000000      0x0000000000000000
0x603390 <host_table+80>:       0x0000000000000000      0x0000000000000000
0x6033a0 <host_table+96>:       0x0000000000000000      0x0000000000000000
0x6033b0 <host_table+112>:      0x0000000000000000      0x0000000000000000
0x6033c0 <host_table+128>:      0x0000000000000000      0x0000000000000000
0x6033d0 <host_table+144>:      0x0000000000000000      0x0000000000000000
0x6033e0 <host_table+160>:      0x0000000000000000      0x0000000000000000
0x6033f0 <host_table+176>:      0x0000000000000000      0x0000000000000000
0x603400 <host_table+192>:      0x0000000000000000      0x0000000000000000
0x603410 <host_table+208>:      0x0000000000000000      0x0000000000000000
0x603420 <host_table+224>:      0x0000000000000000      0x0000000000000000
0x603430 <host_table+240>:      0x0000000000000000      0x0000000000000000
0x603440 <host_table+256>:      0x0000000000000000      0x0000000000000000
0x603450 <host_table+272>:      0x0000000000000000      0x0000000000000000
0x603460 <host_table+288>:      0x0000000000000000      0x0000000000000000
0x603470 <host_table+304>:      0x0000000000000000      0x0000000000000000
0x603480 <host_table+320>:      0x0000000000000000      0x0000000000000000
0x603490 <host_table+336>:      0x0000000000000000      0x0000000000000000
0x6034a0 <host_table+352>:      0x0000000000000000      0x0000000000000000
0x6034b0 <host_table+368>:      0x0000000000000000      0x0000000000000000
0x6034c0 <host_table+384>:      0x0000000000000000      0x0000000000000000
0x6034d0 <host_table+400>:      0x0000000000000000      0x0000000000000000
0x6034e0 <host_table+416>:      0x0000000000000000      0x0000000000000000
```


### c-like code

```c
fun7(esi, void *rdi){
  if(rdi == 0) return -1;
  if(*rdi <= esi ){
    if(*rdi == esi) return 0;                   // step 1
    else a = fun7(esi, *(rdi+16));
    return 2*a+1;                               // step 2
  } else  return 2 * fun7(esi, *(rdi+8));       // step 3
}
```


## reference

1. [CSAPP Lab -- Bomb Lab](https://zhuanlan.zhihu.com/p/36614408)
2. [CSAPP: Bomb Lab 详细实验解析](https://juejin.cn/post/6874568541229334541)
3. [CSAPP实验之Bomb Lab详解](https://blog.csdn.net/qq_38537503/article/details/117199006)
4. [深入理解计算机系统（CS:APP) - Bomb Lab详解](https://www.jianshu.com/p/33eb51b2024e)