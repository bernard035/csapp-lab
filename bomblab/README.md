This is an x86-64 bomb for self-study students. 

## 准备工作

```shell
objdump -d ./bomb >> bomb.s
gdb bomb
```

## bomb1

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

这里把0x402400加载进%esi作为函数参数，然后调用了string_not_equal函数进行判断


```gdb
(gdb) print (char*)0x402400
$1 = 0x402400 "Border relations with Canada have never been better."
```

得到答案
