## A

### sum

```s
# Execution begins at address 0
        .pos 0
        irmovq stack,%rsp
        call main
        halt

# Sample linked list
        .align 8
        ele1:
        .quad 0x00a
        .quad ele2
        ele2:
        .quad 0x0b0
        .quad ele3
        ele3:
        .quad 0xc00
        .quad 0

main:	irmovq ele1,%rdi
        call sum_list
	    ret

sum_list:
	    irmovq $0,%rax
	    jmp test
loop:	mrmovq 0(%rdi),%rsi
        addq %rsi,%rax
        mrmovq 8(%rdi),%rsi
        rrmovq %rsi,%rdi
test:   andq %rdi,%rdi
        jne loop
        ret

        .pos 0x100
stack:

```

### rsum

```s
# rsum.ys by name1e5s
# Execution begins at address 0

	.pos 0
	irmovq stack, %rsp
	call main
	halt

# Sample linked list
	    .align 8
	    ele1:
	    .quad 0x00a
	    .quad e1e2
	    e1e2:
	    .quad 0x0b0
	    .quad e1e3
	    e1e3:
	    .quad 0xc00
	    .quad 0

main:	irmovq ele1, %rdi
	    call rsum_list
	    ret


rsum_list:
	    pushq %r12
	    irmovq $0, %rax
	    andq %rdi,%rdi
	    je re
	    mrmovq 0(%rdi), %r12
	    mrmovq 8(%rdi), %rdi
	    call rsum_list
	    addq %r12, %rax
re:	    popq %r12
	    ret

	    .pos 0x100
stack:
```

```bash
bernard@aqua:~/code/csapp/archlab/solutions$ ../sim/misc/yas sum.ys 
bernard@aqua:~/code/csapp/archlab/solutions$ ../sim/misc/yis sum.yo
Stopped in 29 steps at PC = 0x13.  Status 'HLT', CC Z=1 S=0 O=0
Changes to registers:
%rax:   0x0000000000000000      0x0000000000000cba
%rsp:   0x0000000000000000      0x0000000000000100

Changes to memory:
0x00f0: 0x0000000000000000      0x000000000000005b
0x00f8: 0x0000000000000000      0x0000000000000013
```

```bash
bernard@aqua:~/code/csapp/archlab/solutions$ ../sim/misc/yas rsum.ys 
bernard@aqua:~/code/csapp/archlab/solutions$ ../sim/misc/yas rsum.yo
Usage: ../sim/misc/yas [-V[n]] file.ys
   -V[n]  Generate memory initialization in Verilog format (n-way blocking)
bernard@aqua:~/code/csapp/archlab/solutions$ ../sim/misc/yis rsum.yo
Stopped in 42 steps at PC = 0x13.  Status 'HLT', CC Z=0 S=0 O=0
Changes to registers:
%rax:   0x0000000000000000      0x0000000000000cba
%rsp:   0x0000000000000000      0x0000000000000100

Changes to memory:
0x00b8: 0x0000000000000000      0x0000000000000c00
0x00c0: 0x0000000000000000      0x0000000000000090
0x00c8: 0x0000000000000000      0x00000000000000b0
0x00d0: 0x0000000000000000      0x0000000000000090
0x00d8: 0x0000000000000000      0x000000000000000a
0x00e0: 0x0000000000000000      0x0000000000000090
0x00f0: 0x0000000000000000      0x000000000000005b
0x00f8: 0x0000000000000000      0x0000000000000013
```

### cblock

```s
# Execution begins at address 0
	.pos 0
	irmovq stack, %rsp
	call main
	halt

	.align 8
# Source block
src:
	.quad 0x00a
	.quad 0x0b0
	.quad 0xc00

# Destination block
dest:
	.quad 0x111
	.quad 0x222
	.quad 0x333

main:
	irmovq src, %rdi
	irmovq dest, %rsi
	irmovq $3, %rdx
	call copy_block
	ret

copy_block:
	pushq %r12
	pushq %r13
	pushq %r14
	irmovq $1, %r13
	irmovq $8, %r14
	irmovq $0, %rax 
	jmp Tloop
loop:
	mrmovq 0(%rdi), %r12 
	addq %r14, %rdi
	rmmovq %r12, (%rsi)
	addq %r14, %rsi
	xorq %r12, %rax
	subq %r13, %rdx
Tloop:
	andq %rdx, %rdx
	jg loop
	popq %r14
	popq %r13
	popq %r12
	ret

	.pos 0x100
stack:

```

```bash
bernard@aqua:~/code/csapp/archlab/solutions$ ../sim/misc/yas cblock.ys
bernard@aqua:~/code/csapp/archlab/solutions$ ../sim/misc/yis cblock.yo
Stopped in 45 steps at PC = 0x13.  Status 'HLT', CC Z=1 S=0 O=0
Changes to registers:
%rax:   0x0000000000000000      0x0000000000000cba
%rsp:   0x0000000000000000      0x0000000000000100
%rsi:   0x0000000000000000      0x0000000000000048
%rdi:   0x0000000000000000      0x0000000000000030

Changes to memory:
0x0030: 0x0000000000000111      0x000000000000000a
0x0038: 0x0000000000000222      0x00000000000000b0
0x0040: 0x0000000000000333      0x0000000000000c00
0x00f0: 0x0000000000000000      0x000000000000006f
0x00f8: 0x0000000000000000      0x0000000000000013
```

## B

```bash
bernard@aqua:~/code/csapp/archlab/sim/seq$ ./ssim -t ../y86-code/asumi.yo
Y86-64 Processor: seq-full.hcl
137 bytes of code read
IF: Fetched irmovq at 0x0.  ra=----, rb=%rsp, valC = 0x100
IF: Fetched call at 0xa.  ra=----, rb=----, valC = 0x38
Wrote 0x13 to address 0xf8
IF: Fetched irmovq at 0x38.  ra=----, rb=%rdi, valC = 0x18
IF: Fetched irmovq at 0x42.  ra=----, rb=%rsi, valC = 0x4
IF: Fetched call at 0x4c.  ra=----, rb=----, valC = 0x56
Wrote 0x55 to address 0xf0
IF: Fetched xorq at 0x56.  ra=%rax, rb=%rax, valC = 0x0
IF: Fetched andq at 0x58.  ra=%rsi, rb=%rsi, valC = 0x0
IF: Fetched jmp at 0x5a.  ra=----, rb=----, valC = 0x83
IF: Fetched jne at 0x83.  ra=----, rb=----, valC = 0x63
IF: Fetched mrmovq at 0x63.  ra=%r10, rb=%rdi, valC = 0x0
IF: Fetched addq at 0x6d.  ra=%r10, rb=%rax, valC = 0x0
IF: Fetched iaddq at 0x6f.  ra=----, rb=%rdi, valC = 0x8
IF: Fetched iaddq at 0x79.  ra=----, rb=%rsi, valC = 0xffffffffffffffff
IF: Fetched jne at 0x83.  ra=----, rb=----, valC = 0x63
IF: Fetched mrmovq at 0x63.  ra=%r10, rb=%rdi, valC = 0x0
IF: Fetched addq at 0x6d.  ra=%r10, rb=%rax, valC = 0x0
IF: Fetched iaddq at 0x6f.  ra=----, rb=%rdi, valC = 0x8
IF: Fetched iaddq at 0x79.  ra=----, rb=%rsi, valC = 0xffffffffffffffff
IF: Fetched jne at 0x83.  ra=----, rb=----, valC = 0x63
IF: Fetched mrmovq at 0x63.  ra=%r10, rb=%rdi, valC = 0x0
IF: Fetched addq at 0x6d.  ra=%r10, rb=%rax, valC = 0x0
IF: Fetched iaddq at 0x6f.  ra=----, rb=%rdi, valC = 0x8
IF: Fetched iaddq at 0x79.  ra=----, rb=%rsi, valC = 0xffffffffffffffff
IF: Fetched jne at 0x83.  ra=----, rb=----, valC = 0x63
IF: Fetched mrmovq at 0x63.  ra=%r10, rb=%rdi, valC = 0x0
IF: Fetched addq at 0x6d.  ra=%r10, rb=%rax, valC = 0x0
IF: Fetched iaddq at 0x6f.  ra=----, rb=%rdi, valC = 0x8
IF: Fetched iaddq at 0x79.  ra=----, rb=%rsi, valC = 0xffffffffffffffff
IF: Fetched jne at 0x83.  ra=----, rb=----, valC = 0x63
IF: Fetched ret at 0x8c.  ra=----, rb=----, valC = 0x0
IF: Fetched ret at 0x55.  ra=----, rb=----, valC = 0x0
IF: Fetched halt at 0x13.  ra=----, rb=----, valC = 0x0
32 instructions executed
Status = HLT
Condition Codes: Z=1 S=0 O=0
Changed Register State:
%rax:   0x0000000000000000      0x0000abcdabcdabcd
%rsp:   0x0000000000000000      0x0000000000000100
%rdi:   0x0000000000000000      0x0000000000000038
%r10:   0x0000000000000000      0x0000a000a000a000
Changed Memory State:
0x00f0: 0x0000000000000000      0x0000000000000055
0x00f8: 0x0000000000000000      0x0000000000000013
ISA Check Succeeds
bernard@aqua:~/code/csapp/archlab/sim/seq$ cd ../y86-code; make testssim
../seq/ssim -t asum.yo > asum.seq
../seq/ssim -t asumr.yo > asumr.seq
../seq/ssim -t cjr.yo > cjr.seq
../seq/ssim -t j-cc.yo > j-cc.seq
../seq/ssim -t poptest.yo > poptest.seq
../seq/ssim -t pushquestion.yo > pushquestion.seq
../seq/ssim -t pushtest.yo > pushtest.seq
../seq/ssim -t prog1.yo > prog1.seq
../seq/ssim -t prog2.yo > prog2.seq
../seq/ssim -t prog3.yo > prog3.seq
../seq/ssim -t prog4.yo > prog4.seq
../seq/ssim -t prog5.yo > prog5.seq
../seq/ssim -t prog6.yo > prog6.seq
../seq/ssim -t prog7.yo > prog7.seq
../seq/ssim -t prog8.yo > prog8.seq
../seq/ssim -t ret-hazard.yo > ret-hazard.seq
grep "ISA Check" *.seq
asum.seq:ISA Check Succeeds
asumr.seq:ISA Check Succeeds
cjr.seq:ISA Check Succeeds
j-cc.seq:ISA Check Succeeds
poptest.seq:ISA Check Succeeds
prog1.seq:ISA Check Succeeds
prog2.seq:ISA Check Succeeds
prog3.seq:ISA Check Succeeds
prog4.seq:ISA Check Succeeds
prog5.seq:ISA Check Succeeds
prog6.seq:ISA Check Succeeds
prog7.seq:ISA Check Succeeds
prog8.seq:ISA Check Succeeds
pushquestion.seq:ISA Check Succeeds
pushtest.seq:ISA Check Succeeds
ret-hazard.seq:ISA Check Succeeds
rm asum.seq asumr.seq cjr.seq j-cc.seq poptest.seq pushquestion.seq pushtest.seq prog1.seq prog2.seq prog3.seq prog4.seq prog5.seq prog6.seq prog7.seq prog8.seq ret-hazard.seq
bernard@aqua:~/code/csapp/archlab/sim/y86-code$ cd ../ptest;make SIM=../seq/ssim
./optest.pl -s ../seq/ssim 
Simulating with ../seq/ssim
  All 49 ISA Checks Succeed
./jtest.pl -s ../seq/ssim 
Simulating with ../seq/ssim
  All 64 ISA Checks Succeed
./ctest.pl -s ../seq/ssim 
Simulating with ../seq/ssim
  All 22 ISA Checks Succeed
./htest.pl -s ../seq/ssim 
Simulating with ../seq/ssim
  All 600 ISA Checks Succeed
bernard@aqua:~/code/csapp/archlab/sim/ptest$ make SIM=../seq/ssim TFLAGS=-i
./optest.pl -s ../seq/ssim -i
Simulating with ../seq/ssim
  All 58 ISA Checks Succeed
./jtest.pl -s ../seq/ssim -i
Simulating with ../seq/ssim
  All 96 ISA Checks Succeed
./ctest.pl -s ../seq/ssim -i
Simulating with ../seq/ssim
  All 22 ISA Checks Succeed
./htest.pl -s ../seq/ssim -i
Simulating with ../seq/ssim
  All 756 ISA Checks Succeed
```

## C

## reference

1. [Lab4 CSAPP: Archlab 高分(56.9/60.0)通过并配好Ubuntu20.04环境](https://zhuanlan.zhihu.com/p/454779772)
2. [CSAPP : Arch Lab 解题报告](https://zhuanlan.zhihu.com/p/36793761)
3. [CSAPP-Lab04 Architecture Lab 深入解析](https://zhuanlan.zhihu.com/p/480380496)