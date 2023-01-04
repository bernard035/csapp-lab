# csapp-lab

这是我的《深入理解计算机系统》[配套实验](http://csapp.cs.cmu.edu/3e/labs.html)代码。

除了符合题目要求，并通过了全部的测试用例以外，我尽可能采用了优雅的实现和详细的注释。

## data-lab

这个lab是对基础数字逻辑中的数据存储部分完成高级语言上的实现。

在这个lab中，我基于纯位运算实现了基础的位运算本身（异或、逻辑非）、条件判断、浮点数转化等基础运算，同时关注代码效率和优雅简洁的实现。

相较于网上的其他实现，我充分考虑了undefined behavior，使代码在Clang和GCC上都能得到正确的结果。

收获：对计算机补码，二进制操作有深刻的认识，对浮点数存储方式有清晰的了解。

## bomb-lab

这个实验是通过反汇编一个可执行文件，分析汇编代码来找到六个炸弹的密码。

完成实验需要熟悉基础的汇编知识，掌握GDB的使用以及一些编译的基础知识。

| 关卡  |                           考察内容                           |
| :---: | :----------------------------------------------------------: |
|   1   |                  字符串常量的存储和传递方式                  |
|   2   |                     数组、循环的汇编实现                     |
|   3   |               switch-case的内存结构和汇编实现                |
|   4   |                  递归（二分搜索）的汇编实现                  |
|   5   |                栈破坏检测、循环汇编代码的分析                |
|   6   | 结构体、链表在内存中的结构与汇编实现、多重循环汇编代码的分析 |
| 隐藏  |                  二叉树的内存结构和汇编使用                  |

我完成了详尽的说明文档，包括对反汇编代码的注释、分析过程和解释以及`c-like`代码的转写，所有存在多解的题目都提供了多解。