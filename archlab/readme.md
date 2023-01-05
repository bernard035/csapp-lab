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


```bash
bernard@aqua:~/code/csapp/archlab/sim/pipe$ ./psim -t ../y86-code/asumi.yo
Y86-64 Processor: pipe-full.hcl
137 bytes of code read

Cycle 0. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x0
D: instr = nop, rA = ----, rB = ----, valC = 0x0, valP = 0x0, Stat = BUB
E: instr = nop, valC = 0x0, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = BUB
M: instr = nop, Cnd = 0, valE = 0x0, valA = 0x0
   dstE = ----, dstM = ----, Stat = BUB
W: instr = nop, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = BUB
        Fetch: f_pc = 0x0, imem_instr = irmovq, f_instr = irmovq
        Execute: ALU: + 0x0 0x0 --> 0x0

Cycle 1. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0xa
D: instr = irmovq, rA = ----, rB = %rsp, valC = 0x100, valP = 0xa, Stat = AOK
E: instr = nop, valC = 0x0, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = BUB
M: instr = nop, Cnd = 1, valE = 0x0, valA = 0x0
   dstE = ----, dstM = ----, Stat = BUB
W: instr = nop, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = BUB
        Fetch: f_pc = 0xa, imem_instr = call, f_instr = call
        Execute: ALU: + 0x0 0x0 --> 0x0

Cycle 2. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x38
D: instr = call, rA = ----, rB = ----, valC = 0x38, valP = 0x13, Stat = AOK
E: instr = irmovq, valC = 0x100, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = %rsp, dstM = ----, Stat = AOK
M: instr = nop, Cnd = 1, valE = 0x0, valA = 0x0
   dstE = ----, dstM = ----, Stat = BUB
W: instr = nop, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = BUB
        Fetch: f_pc = 0x38, imem_instr = irmovq, f_instr = irmovq
        Execute: ALU: + 0x100 0x0 --> 0x100

Cycle 3. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x42
D: instr = irmovq, rA = ----, rB = %rdi, valC = 0x18, valP = 0x42, Stat = AOK
E: instr = call, valC = 0x38, valA = 0x13, valB = 0x100
   srcA = ----, srcB = %rsp, dstE = %rsp, dstM = ----, Stat = AOK
M: instr = irmovq, Cnd = 1, valE = 0x100, valA = 0x0
   dstE = %rsp, dstM = ----, Stat = AOK
W: instr = nop, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = BUB
        Fetch: f_pc = 0x42, imem_instr = irmovq, f_instr = irmovq
        Execute: ALU: + 0xfffffffffffffff8 0x100 --> 0xf8

Cycle 4. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x4c
D: instr = irmovq, rA = ----, rB = %rsi, valC = 0x4, valP = 0x4c, Stat = AOK
E: instr = irmovq, valC = 0x18, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = %rdi, dstM = ----, Stat = AOK
M: instr = call, Cnd = 1, valE = 0xf8, valA = 0x13
   dstE = %rsp, dstM = ----, Stat = AOK
W: instr = irmovq, valE = 0x100, valM = 0x0, dstE = %rsp, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x4c, imem_instr = call, f_instr = call
        Execute: ALU: + 0x18 0x0 --> 0x18
        Writeback: Wrote 0x100 to register %rsp
        Wrote 0x13 to address 0xf8

Cycle 5. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x56
D: instr = call, rA = ----, rB = ----, valC = 0x56, valP = 0x55, Stat = AOK
E: instr = irmovq, valC = 0x4, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = %rsi, dstM = ----, Stat = AOK
M: instr = irmovq, Cnd = 1, valE = 0x18, valA = 0x0
   dstE = %rdi, dstM = ----, Stat = AOK
W: instr = call, valE = 0xf8, valM = 0x0, dstE = %rsp, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x56, imem_instr = xorq, f_instr = xorq
        Execute: ALU: + 0x4 0x0 --> 0x4
        Writeback: Wrote 0xf8 to register %rsp

Cycle 6. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x58
D: instr = xorq, rA = %rax, rB = %rax, valC = 0x0, valP = 0x58, Stat = AOK
E: instr = call, valC = 0x56, valA = 0x55, valB = 0xf8
   srcA = ----, srcB = %rsp, dstE = %rsp, dstM = ----, Stat = AOK
M: instr = irmovq, Cnd = 1, valE = 0x4, valA = 0x0
   dstE = %rsi, dstM = ----, Stat = AOK
W: instr = irmovq, valE = 0x18, valM = 0x0, dstE = %rdi, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x58, imem_instr = andq, f_instr = andq
        Execute: ALU: + 0xfffffffffffffff8 0xf8 --> 0xf0
        Writeback: Wrote 0x18 to register %rdi

Cycle 7. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x5a
D: instr = andq, rA = %rsi, rB = %rsi, valC = 0x0, valP = 0x5a, Stat = AOK
E: instr = xorq, valC = 0x0, valA = 0x0, valB = 0x0
   srcA = %rax, srcB = %rax, dstE = %rax, dstM = ----, Stat = AOK
M: instr = call, Cnd = 1, valE = 0xf0, valA = 0x55
   dstE = %rsp, dstM = ----, Stat = AOK
W: instr = irmovq, valE = 0x4, valM = 0x0, dstE = %rsi, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x5a, imem_instr = jmp, f_instr = jmp
        Execute: ALU: ^ 0x0 0x0 --> 0x0
        Execute: New cc = Z=1 S=0 O=0
        Writeback: Wrote 0x4 to register %rsi
        Wrote 0x55 to address 0xf0

Cycle 8. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x83
D: instr = jmp, rA = ----, rB = ----, valC = 0x83, valP = 0x63, Stat = AOK
E: instr = andq, valC = 0x0, valA = 0x4, valB = 0x4
   srcA = %rsi, srcB = %rsi, dstE = %rsi, dstM = ----, Stat = AOK
M: instr = xorq, Cnd = 1, valE = 0x0, valA = 0x0
   dstE = %rax, dstM = ----, Stat = AOK
W: instr = call, valE = 0xf0, valM = 0x0, dstE = %rsp, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x83, imem_instr = jne, f_instr = jne
        Execute: ALU: & 0x4 0x4 --> 0x4
        Execute: New cc = Z=0 S=0 O=0
        Writeback: Wrote 0xf0 to register %rsp

Cycle 9. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x63
D: instr = jne, rA = ----, rB = ----, valC = 0x63, valP = 0x8c, Stat = AOK
E: instr = jmp, valC = 0x83, valA = 0x63, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = AOK
M: instr = andq, Cnd = 0, valE = 0x4, valA = 0x4
   dstE = %rsi, dstM = ----, Stat = AOK
W: instr = xorq, valE = 0x0, valM = 0x0, dstE = %rax, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x63, imem_instr = mrmovq, f_instr = mrmovq
        Execute: instr = jmp, cc = Z=0 S=0 O=0, branch taken
        Execute: ALU: + 0x0 0x0 --> 0x0
        Writeback: Wrote 0x0 to register %rax

Cycle 10. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x6d
D: instr = mrmovq, rA = %r10, rB = %rdi, valC = 0x0, valP = 0x6d, Stat = AOK
E: instr = jne, valC = 0x63, valA = 0x8c, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = AOK
M: instr = jmp, Cnd = 1, valE = 0x0, valA = 0x63
   dstE = ----, dstM = ----, Stat = AOK
W: instr = andq, valE = 0x4, valM = 0x0, dstE = %rsi, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x6d, imem_instr = addq, f_instr = addq
        Execute: instr = jne, cc = Z=0 S=0 O=0, branch taken
        Execute: ALU: + 0x0 0x0 --> 0x0
        Writeback: Wrote 0x4 to register %rsi

Cycle 11. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x6f
D: instr = addq, rA = %r10, rB = %rax, valC = 0x0, valP = 0x6f, Stat = AOK
E: instr = mrmovq, valC = 0x0, valA = 0x0, valB = 0x18
   srcA = ----, srcB = %rdi, dstE = ----, dstM = %r10, Stat = AOK
M: instr = jne, Cnd = 1, valE = 0x0, valA = 0x8c
   dstE = ----, dstM = ----, Stat = AOK
W: instr = jmp, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x6f, imem_instr = iaddq, f_instr = iaddq
        Execute: ALU: + 0x0 0x18 --> 0x18

Cycle 12. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x6f
D: instr = addq, rA = %r10, rB = %rax, valC = 0x0, valP = 0x6f, Stat = AOK
E: instr = nop, valC = 0x0, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = BUB
M: instr = mrmovq, Cnd = 1, valE = 0x18, valA = 0x0
   dstE = ----, dstM = %r10, Stat = AOK
W: instr = jne, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x6f, imem_instr = iaddq, f_instr = iaddq
        Memory: Read 0xd000d000d from 0x18
        Execute: ALU: + 0x0 0x0 --> 0x0

Cycle 13. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x79
D: instr = iaddq, rA = ----, rB = %rdi, valC = 0x8, valP = 0x79, Stat = AOK
E: instr = addq, valC = 0x0, valA = 0xd000d000d, valB = 0x0
   srcA = %r10, srcB = %rax, dstE = %rax, dstM = ----, Stat = AOK
M: instr = nop, Cnd = 1, valE = 0x0, valA = 0x0
   dstE = ----, dstM = ----, Stat = BUB
W: instr = mrmovq, valE = 0x18, valM = 0xd000d000d, dstE = ----, dstM = %r10, Stat = AOK
        Fetch: f_pc = 0x79, imem_instr = iaddq, f_instr = iaddq
        Execute: ALU: + 0xd000d000d 0x0 --> 0xd000d000d
        Execute: New cc = Z=0 S=0 O=0
        Writeback: Wrote 0xd000d000d to register %r10

Cycle 14. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x83
D: instr = iaddq, rA = ----, rB = %rsi, valC = 0xffffffffffffffff, valP = 0x83, Stat = AOK
E: instr = iaddq, valC = 0x8, valA = 0x0, valB = 0x18
   srcA = ----, srcB = %rdi, dstE = %rdi, dstM = ----, Stat = AOK
M: instr = addq, Cnd = 1, valE = 0xd000d000d, valA = 0xd000d000d
   dstE = %rax, dstM = ----, Stat = AOK
W: instr = nop, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = BUB
        Fetch: f_pc = 0x83, imem_instr = jne, f_instr = jne
        Execute: ALU: + 0x8 0x18 --> 0x20
        Execute: New cc = Z=0 S=0 O=0

Cycle 15. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x63
D: instr = jne, rA = ----, rB = ----, valC = 0x63, valP = 0x8c, Stat = AOK
E: instr = iaddq, valC = 0xffffffffffffffff, valA = 0x0, valB = 0x4
   srcA = ----, srcB = %rsi, dstE = %rsi, dstM = ----, Stat = AOK
M: instr = iaddq, Cnd = 1, valE = 0x20, valA = 0x0
   dstE = %rdi, dstM = ----, Stat = AOK
W: instr = addq, valE = 0xd000d000d, valM = 0x0, dstE = %rax, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x63, imem_instr = mrmovq, f_instr = mrmovq
        Execute: ALU: + 0xffffffffffffffff 0x4 --> 0x3
        Execute: New cc = Z=0 S=0 O=0
        Writeback: Wrote 0xd000d000d to register %rax

Cycle 16. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x6d
D: instr = mrmovq, rA = %r10, rB = %rdi, valC = 0x0, valP = 0x6d, Stat = AOK
E: instr = jne, valC = 0x63, valA = 0x8c, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = AOK
M: instr = iaddq, Cnd = 1, valE = 0x3, valA = 0x0
   dstE = %rsi, dstM = ----, Stat = AOK
W: instr = iaddq, valE = 0x20, valM = 0x0, dstE = %rdi, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x6d, imem_instr = addq, f_instr = addq
        Execute: instr = jne, cc = Z=0 S=0 O=0, branch taken
        Execute: ALU: + 0x0 0x0 --> 0x0
        Writeback: Wrote 0x20 to register %rdi

Cycle 17. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x6f
D: instr = addq, rA = %r10, rB = %rax, valC = 0x0, valP = 0x6f, Stat = AOK
E: instr = mrmovq, valC = 0x0, valA = 0x0, valB = 0x20
   srcA = ----, srcB = %rdi, dstE = ----, dstM = %r10, Stat = AOK
M: instr = jne, Cnd = 1, valE = 0x0, valA = 0x8c
   dstE = ----, dstM = ----, Stat = AOK
W: instr = iaddq, valE = 0x3, valM = 0x0, dstE = %rsi, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x6f, imem_instr = iaddq, f_instr = iaddq
        Execute: ALU: + 0x0 0x20 --> 0x20
        Writeback: Wrote 0x3 to register %rsi

Cycle 18. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x6f
D: instr = addq, rA = %r10, rB = %rax, valC = 0x0, valP = 0x6f, Stat = AOK
E: instr = nop, valC = 0x0, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = BUB
M: instr = mrmovq, Cnd = 1, valE = 0x20, valA = 0x0
   dstE = ----, dstM = %r10, Stat = AOK
W: instr = jne, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x6f, imem_instr = iaddq, f_instr = iaddq
        Memory: Read 0xc000c000c0 from 0x20
        Execute: ALU: + 0x0 0x0 --> 0x0

Cycle 19. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x79
D: instr = iaddq, rA = ----, rB = %rdi, valC = 0x8, valP = 0x79, Stat = AOK
E: instr = addq, valC = 0x0, valA = 0xc000c000c0, valB = 0xd000d000d
   srcA = %r10, srcB = %rax, dstE = %rax, dstM = ----, Stat = AOK
M: instr = nop, Cnd = 1, valE = 0x0, valA = 0x0
   dstE = ----, dstM = ----, Stat = BUB
W: instr = mrmovq, valE = 0x20, valM = 0xc000c000c0, dstE = ----, dstM = %r10, Stat = AOK
        Fetch: f_pc = 0x79, imem_instr = iaddq, f_instr = iaddq
        Execute: ALU: + 0xc000c000c0 0xd000d000d --> 0xcd00cd00cd
        Execute: New cc = Z=0 S=0 O=0
        Writeback: Wrote 0xc000c000c0 to register %r10

Cycle 20. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x83
D: instr = iaddq, rA = ----, rB = %rsi, valC = 0xffffffffffffffff, valP = 0x83, Stat = AOK
E: instr = iaddq, valC = 0x8, valA = 0x0, valB = 0x20
   srcA = ----, srcB = %rdi, dstE = %rdi, dstM = ----, Stat = AOK
M: instr = addq, Cnd = 1, valE = 0xcd00cd00cd, valA = 0xc000c000c0
   dstE = %rax, dstM = ----, Stat = AOK
W: instr = nop, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = BUB
        Fetch: f_pc = 0x83, imem_instr = jne, f_instr = jne
        Execute: ALU: + 0x8 0x20 --> 0x28
        Execute: New cc = Z=0 S=0 O=0

Cycle 21. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x63
D: instr = jne, rA = ----, rB = ----, valC = 0x63, valP = 0x8c, Stat = AOK
E: instr = iaddq, valC = 0xffffffffffffffff, valA = 0x0, valB = 0x3
   srcA = ----, srcB = %rsi, dstE = %rsi, dstM = ----, Stat = AOK
M: instr = iaddq, Cnd = 1, valE = 0x28, valA = 0x0
   dstE = %rdi, dstM = ----, Stat = AOK
W: instr = addq, valE = 0xcd00cd00cd, valM = 0x0, dstE = %rax, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x63, imem_instr = mrmovq, f_instr = mrmovq
        Execute: ALU: + 0xffffffffffffffff 0x3 --> 0x2
        Execute: New cc = Z=0 S=0 O=0
        Writeback: Wrote 0xcd00cd00cd to register %rax

Cycle 22. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x6d
D: instr = mrmovq, rA = %r10, rB = %rdi, valC = 0x0, valP = 0x6d, Stat = AOK
E: instr = jne, valC = 0x63, valA = 0x8c, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = AOK
M: instr = iaddq, Cnd = 1, valE = 0x2, valA = 0x0
   dstE = %rsi, dstM = ----, Stat = AOK
W: instr = iaddq, valE = 0x28, valM = 0x0, dstE = %rdi, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x6d, imem_instr = addq, f_instr = addq
        Execute: instr = jne, cc = Z=0 S=0 O=0, branch taken
        Execute: ALU: + 0x0 0x0 --> 0x0
        Writeback: Wrote 0x28 to register %rdi

Cycle 23. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x6f
D: instr = addq, rA = %r10, rB = %rax, valC = 0x0, valP = 0x6f, Stat = AOK
E: instr = mrmovq, valC = 0x0, valA = 0x0, valB = 0x28
   srcA = ----, srcB = %rdi, dstE = ----, dstM = %r10, Stat = AOK
M: instr = jne, Cnd = 1, valE = 0x0, valA = 0x8c
   dstE = ----, dstM = ----, Stat = AOK
W: instr = iaddq, valE = 0x2, valM = 0x0, dstE = %rsi, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x6f, imem_instr = iaddq, f_instr = iaddq
        Execute: ALU: + 0x0 0x28 --> 0x28
        Writeback: Wrote 0x2 to register %rsi

Cycle 24. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x6f
D: instr = addq, rA = %r10, rB = %rax, valC = 0x0, valP = 0x6f, Stat = AOK
E: instr = nop, valC = 0x0, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = BUB
M: instr = mrmovq, Cnd = 1, valE = 0x28, valA = 0x0
   dstE = ----, dstM = %r10, Stat = AOK
W: instr = jne, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x6f, imem_instr = iaddq, f_instr = iaddq
        Memory: Read 0xb000b000b00 from 0x28
        Execute: ALU: + 0x0 0x0 --> 0x0

Cycle 25. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x79
D: instr = iaddq, rA = ----, rB = %rdi, valC = 0x8, valP = 0x79, Stat = AOK
E: instr = addq, valC = 0x0, valA = 0xb000b000b00, valB = 0xcd00cd00cd
   srcA = %r10, srcB = %rax, dstE = %rax, dstM = ----, Stat = AOK
M: instr = nop, Cnd = 1, valE = 0x0, valA = 0x0
   dstE = ----, dstM = ----, Stat = BUB
W: instr = mrmovq, valE = 0x28, valM = 0xb000b000b00, dstE = ----, dstM = %r10, Stat = AOK
        Fetch: f_pc = 0x79, imem_instr = iaddq, f_instr = iaddq
        Execute: ALU: + 0xb000b000b00 0xcd00cd00cd --> 0xbcd0bcd0bcd
        Execute: New cc = Z=0 S=0 O=0
        Writeback: Wrote 0xb000b000b00 to register %r10

Cycle 26. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x83
D: instr = iaddq, rA = ----, rB = %rsi, valC = 0xffffffffffffffff, valP = 0x83, Stat = AOK
E: instr = iaddq, valC = 0x8, valA = 0x0, valB = 0x28
   srcA = ----, srcB = %rdi, dstE = %rdi, dstM = ----, Stat = AOK
M: instr = addq, Cnd = 1, valE = 0xbcd0bcd0bcd, valA = 0xb000b000b00
   dstE = %rax, dstM = ----, Stat = AOK
W: instr = nop, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = BUB
        Fetch: f_pc = 0x83, imem_instr = jne, f_instr = jne
        Execute: ALU: + 0x8 0x28 --> 0x30
        Execute: New cc = Z=0 S=0 O=0

Cycle 27. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x63
D: instr = jne, rA = ----, rB = ----, valC = 0x63, valP = 0x8c, Stat = AOK
E: instr = iaddq, valC = 0xffffffffffffffff, valA = 0x0, valB = 0x2
   srcA = ----, srcB = %rsi, dstE = %rsi, dstM = ----, Stat = AOK
M: instr = iaddq, Cnd = 1, valE = 0x30, valA = 0x0
   dstE = %rdi, dstM = ----, Stat = AOK
W: instr = addq, valE = 0xbcd0bcd0bcd, valM = 0x0, dstE = %rax, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x63, imem_instr = mrmovq, f_instr = mrmovq
        Execute: ALU: + 0xffffffffffffffff 0x2 --> 0x1
        Execute: New cc = Z=0 S=0 O=0
        Writeback: Wrote 0xbcd0bcd0bcd to register %rax

Cycle 28. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x6d
D: instr = mrmovq, rA = %r10, rB = %rdi, valC = 0x0, valP = 0x6d, Stat = AOK
E: instr = jne, valC = 0x63, valA = 0x8c, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = AOK
M: instr = iaddq, Cnd = 1, valE = 0x1, valA = 0x0
   dstE = %rsi, dstM = ----, Stat = AOK
W: instr = iaddq, valE = 0x30, valM = 0x0, dstE = %rdi, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x6d, imem_instr = addq, f_instr = addq
        Execute: instr = jne, cc = Z=0 S=0 O=0, branch taken
        Execute: ALU: + 0x0 0x0 --> 0x0
        Writeback: Wrote 0x30 to register %rdi

Cycle 29. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x6f
D: instr = addq, rA = %r10, rB = %rax, valC = 0x0, valP = 0x6f, Stat = AOK
E: instr = mrmovq, valC = 0x0, valA = 0x0, valB = 0x30
   srcA = ----, srcB = %rdi, dstE = ----, dstM = %r10, Stat = AOK
M: instr = jne, Cnd = 1, valE = 0x0, valA = 0x8c
   dstE = ----, dstM = ----, Stat = AOK
W: instr = iaddq, valE = 0x1, valM = 0x0, dstE = %rsi, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x6f, imem_instr = iaddq, f_instr = iaddq
        Execute: ALU: + 0x0 0x30 --> 0x30
        Writeback: Wrote 0x1 to register %rsi

Cycle 30. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x6f
D: instr = addq, rA = %r10, rB = %rax, valC = 0x0, valP = 0x6f, Stat = AOK
E: instr = nop, valC = 0x0, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = BUB
M: instr = mrmovq, Cnd = 1, valE = 0x30, valA = 0x0
   dstE = ----, dstM = %r10, Stat = AOK
W: instr = jne, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x6f, imem_instr = iaddq, f_instr = iaddq
        Memory: Read 0xa000a000a000 from 0x30
        Execute: ALU: + 0x0 0x0 --> 0x0

Cycle 31. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x79
D: instr = iaddq, rA = ----, rB = %rdi, valC = 0x8, valP = 0x79, Stat = AOK
E: instr = addq, valC = 0x0, valA = 0xa000a000a000, valB = 0xbcd0bcd0bcd
   srcA = %r10, srcB = %rax, dstE = %rax, dstM = ----, Stat = AOK
M: instr = nop, Cnd = 1, valE = 0x0, valA = 0x0
   dstE = ----, dstM = ----, Stat = BUB
W: instr = mrmovq, valE = 0x30, valM = 0xa000a000a000, dstE = ----, dstM = %r10, Stat = AOK
        Fetch: f_pc = 0x79, imem_instr = iaddq, f_instr = iaddq
        Execute: ALU: + 0xa000a000a000 0xbcd0bcd0bcd --> 0xabcdabcdabcd
        Execute: New cc = Z=0 S=0 O=0
        Writeback: Wrote 0xa000a000a000 to register %r10

Cycle 32. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x83
D: instr = iaddq, rA = ----, rB = %rsi, valC = 0xffffffffffffffff, valP = 0x83, Stat = AOK
E: instr = iaddq, valC = 0x8, valA = 0x0, valB = 0x30
   srcA = ----, srcB = %rdi, dstE = %rdi, dstM = ----, Stat = AOK
M: instr = addq, Cnd = 1, valE = 0xabcdabcdabcd, valA = 0xa000a000a000
   dstE = %rax, dstM = ----, Stat = AOK
W: instr = nop, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = BUB
        Fetch: f_pc = 0x83, imem_instr = jne, f_instr = jne
        Execute: ALU: + 0x8 0x30 --> 0x38
        Execute: New cc = Z=0 S=0 O=0

Cycle 33. CC=Z=0 S=0 O=0, Stat=AOK
F: predPC = 0x63
D: instr = jne, rA = ----, rB = ----, valC = 0x63, valP = 0x8c, Stat = AOK
E: instr = iaddq, valC = 0xffffffffffffffff, valA = 0x0, valB = 0x1
   srcA = ----, srcB = %rsi, dstE = %rsi, dstM = ----, Stat = AOK
M: instr = iaddq, Cnd = 1, valE = 0x38, valA = 0x0
   dstE = %rdi, dstM = ----, Stat = AOK
W: instr = addq, valE = 0xabcdabcdabcd, valM = 0x0, dstE = %rax, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x63, imem_instr = mrmovq, f_instr = mrmovq
        Execute: ALU: + 0xffffffffffffffff 0x1 --> 0x0
        Execute: New cc = Z=1 S=0 O=0
        Writeback: Wrote 0xabcdabcdabcd to register %rax

Cycle 34. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x6d
D: instr = mrmovq, rA = %r10, rB = %rdi, valC = 0x0, valP = 0x6d, Stat = AOK
E: instr = jne, valC = 0x63, valA = 0x8c, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = AOK
M: instr = iaddq, Cnd = 1, valE = 0x0, valA = 0x0
   dstE = %rsi, dstM = ----, Stat = AOK
W: instr = iaddq, valE = 0x38, valM = 0x0, dstE = %rdi, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x6d, imem_instr = addq, f_instr = addq
        Execute: instr = jne, cc = Z=1 S=0 O=0, branch not taken
        Execute: ALU: + 0x0 0x0 --> 0x0
        Writeback: Wrote 0x38 to register %rdi

Cycle 35. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x6f
D: instr = nop, rA = ----, rB = ----, valC = 0x0, valP = 0x0, Stat = BUB
E: instr = nop, valC = 0x0, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = BUB
M: instr = jne, Cnd = 0, valE = 0x0, valA = 0x8c
   dstE = ----, dstM = ----, Stat = AOK
W: instr = iaddq, valE = 0x0, valM = 0x0, dstE = %rsi, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x8c, imem_instr = ret, f_instr = ret
        Execute: ALU: + 0x0 0x0 --> 0x0
        Writeback: Wrote 0x0 to register %rsi

Cycle 36. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x8d
D: instr = ret, rA = ----, rB = ----, valC = 0x0, valP = 0x8d, Stat = AOK
E: instr = nop, valC = 0x0, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = BUB
M: instr = nop, Cnd = 1, valE = 0x0, valA = 0x0
   dstE = ----, dstM = ----, Stat = BUB
W: instr = jne, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x8d, imem_instr = halt, f_instr = halt
        Execute: ALU: + 0x0 0x0 --> 0x0

Cycle 37. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x8d
D: instr = nop, rA = ----, rB = ----, valC = 0x0, valP = 0x0, Stat = BUB
E: instr = ret, valC = 0x0, valA = 0xf0, valB = 0xf0
   srcA = %rsp, srcB = %rsp, dstE = %rsp, dstM = ----, Stat = AOK
M: instr = nop, Cnd = 1, valE = 0x0, valA = 0x0
   dstE = ----, dstM = ----, Stat = BUB
W: instr = nop, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = BUB
        Fetch: f_pc = 0x8d, imem_instr = halt, f_instr = halt
        Execute: ALU: + 0x8 0xf0 --> 0xf8

Cycle 38. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x8d
D: instr = nop, rA = ----, rB = ----, valC = 0x0, valP = 0x0, Stat = BUB
E: instr = nop, valC = 0x0, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = BUB
M: instr = ret, Cnd = 1, valE = 0xf8, valA = 0xf0
   dstE = %rsp, dstM = ----, Stat = AOK
W: instr = nop, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = BUB
        Fetch: f_pc = 0x8d, imem_instr = halt, f_instr = halt
        Memory: Read 0x55 from 0xf0
        Execute: ALU: + 0x0 0x0 --> 0x0

Cycle 39. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x8d
D: instr = nop, rA = ----, rB = ----, valC = 0x0, valP = 0x0, Stat = BUB
E: instr = nop, valC = 0x0, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = BUB
M: instr = nop, Cnd = 1, valE = 0x0, valA = 0x0
   dstE = ----, dstM = ----, Stat = BUB
W: instr = ret, valE = 0xf8, valM = 0x55, dstE = %rsp, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x55, imem_instr = ret, f_instr = ret
        Execute: ALU: + 0x0 0x0 --> 0x0
        Writeback: Wrote 0xf8 to register %rsp

Cycle 40. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x56
D: instr = ret, rA = ----, rB = ----, valC = 0x0, valP = 0x56, Stat = AOK
E: instr = nop, valC = 0x0, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = BUB
M: instr = nop, Cnd = 1, valE = 0x0, valA = 0x0
   dstE = ----, dstM = ----, Stat = BUB
W: instr = nop, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = BUB
        Fetch: f_pc = 0x56, imem_instr = xorq, f_instr = xorq
        Execute: ALU: + 0x0 0x0 --> 0x0

Cycle 41. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x56
D: instr = nop, rA = ----, rB = ----, valC = 0x0, valP = 0x0, Stat = BUB
E: instr = ret, valC = 0x0, valA = 0xf8, valB = 0xf8
   srcA = %rsp, srcB = %rsp, dstE = %rsp, dstM = ----, Stat = AOK
M: instr = nop, Cnd = 1, valE = 0x0, valA = 0x0
   dstE = ----, dstM = ----, Stat = BUB
W: instr = nop, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = BUB
        Fetch: f_pc = 0x56, imem_instr = xorq, f_instr = xorq
        Execute: ALU: + 0x8 0xf8 --> 0x100

Cycle 42. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x56
D: instr = nop, rA = ----, rB = ----, valC = 0x0, valP = 0x0, Stat = BUB
E: instr = nop, valC = 0x0, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = BUB
M: instr = ret, Cnd = 1, valE = 0x100, valA = 0xf8
   dstE = %rsp, dstM = ----, Stat = AOK
W: instr = nop, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = BUB
        Fetch: f_pc = 0x56, imem_instr = xorq, f_instr = xorq
        Memory: Read 0x13 from 0xf8
        Execute: ALU: + 0x0 0x0 --> 0x0

Cycle 43. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x56
D: instr = nop, rA = ----, rB = ----, valC = 0x0, valP = 0x0, Stat = BUB
E: instr = nop, valC = 0x0, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = BUB
M: instr = nop, Cnd = 1, valE = 0x0, valA = 0x0
   dstE = ----, dstM = ----, Stat = BUB
W: instr = ret, valE = 0x100, valM = 0x13, dstE = %rsp, dstM = ----, Stat = AOK
        Fetch: f_pc = 0x13, imem_instr = halt, f_instr = halt
        Execute: ALU: + 0x0 0x0 --> 0x0
        Writeback: Wrote 0x100 to register %rsp

Cycle 44. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x14
D: instr = halt, rA = ----, rB = ----, valC = 0x0, valP = 0x14, Stat = HLT
E: instr = nop, valC = 0x0, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = BUB
M: instr = nop, Cnd = 1, valE = 0x0, valA = 0x0
   dstE = ----, dstM = ----, Stat = BUB
W: instr = nop, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = BUB
        Fetch: f_pc = 0x14, imem_instr = halt, f_instr = halt
        Execute: ALU: + 0x0 0x0 --> 0x0

Cycle 45. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x15
D: instr = halt, rA = ----, rB = ----, valC = 0x0, valP = 0x15, Stat = HLT
E: instr = halt, valC = 0x0, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = HLT
M: instr = nop, Cnd = 1, valE = 0x0, valA = 0x0
   dstE = ----, dstM = ----, Stat = BUB
W: instr = nop, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = BUB
        Fetch: f_pc = 0x15, imem_instr = halt, f_instr = halt
        Execute: ALU: + 0x0 0x0 --> 0x0

Cycle 46. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x16
D: instr = halt, rA = ----, rB = ----, valC = 0x0, valP = 0x16, Stat = HLT
E: instr = halt, valC = 0x0, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = HLT
M: instr = halt, Cnd = 1, valE = 0x0, valA = 0x0
   dstE = ----, dstM = ----, Stat = HLT
W: instr = nop, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = BUB
        Fetch: f_pc = 0x16, imem_instr = halt, f_instr = halt
        Execute: ALU: + 0x0 0x0 --> 0x0

Cycle 47. CC=Z=1 S=0 O=0, Stat=AOK
F: predPC = 0x17
D: instr = halt, rA = ----, rB = ----, valC = 0x0, valP = 0x17, Stat = HLT
E: instr = halt, valC = 0x0, valA = 0x0, valB = 0x0
   srcA = ----, srcB = ----, dstE = ----, dstM = ----, Stat = HLT
M: instr = nop, Cnd = 0, valE = 0x0, valA = 0x0
   dstE = ----, dstM = ----, Stat = BUB
W: instr = halt, valE = 0x0, valM = 0x0, dstE = ----, dstM = ----, Stat = HLT
        Fetch: f_pc = 0x17, imem_instr = halt, f_instr = halt
        Execute: ALU: + 0x0 0x0 --> 0x0
48 instructions executed
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
CPI: 44 cycles/32 instructions = 1.38
bernard@aqua:~/code/csapp/archlab/sim/pipe$ cd ../ptest; make SIM=../pipe/psim
./optest.pl -s ../pipe/psim 
Simulating with ../pipe/psim
  All 49 ISA Checks Succeed
./jtest.pl -s ../pipe/psim 
Simulating with ../pipe/psim
  All 64 ISA Checks Succeed
./ctest.pl -s ../pipe/psim 
Simulating with ../pipe/psim
  All 22 ISA Checks Succeed
./htest.pl -s ../pipe/psim 
Simulating with ../pipe/psim
  All 600 ISA Checks Succeed
bernard@aqua:~/code/csapp/archlab/sim/ptest$ make SIM=../pipe/psim TFLAGS=-i
./optest.pl -s ../pipe/psim -i
Simulating with ../pipe/psim
  All 58 ISA Checks Succeed
./jtest.pl -s ../pipe/psim -i
Simulating with ../pipe/psim
  All 96 ISA Checks Succeed
./ctest.pl -s ../pipe/psim -i
Simulating with ../pipe/psim
  All 22 ISA Checks Succeed
./htest.pl -s ../pipe/psim -i
Simulating with ../pipe/psim
  All 756 ISA Checks Succeed
bernard@aqua:~/code/csapp/archlab/sim/ptest$ cd ../pipe/
bernard@aqua:~/code/csapp/archlab/sim/pipe$ ls
benchmark.pl    Makefile         pipe-full.c        pipe-std.c    sdriver.yo
check-len.pl    ncopy.c          pipe-full.hcl      pipe-std.hcl  sdriver.ys
correctness.pl  ncopy.ys         pipe-lf.hcl        pipe.tcl      sim.h
gen-driver.pl   pipe-1w.hcl      pipeline.h         psim          stages.h
ldriver.yo      pipe-broken.hcl  pipe-nobypass.hcl  psim.c
ldriver.ys      pipe-btfnt.hcl   pipe-nt.hcl        README
bernard@aqua:~/code/csapp/archlab/sim/pipe$ ./correctness.pl
Simulating with instruction set simulator yis
        ncopy
0       OK
1       OK
2       OK
3       OK
4       OK
5       OK
6       OK
7       OK
8       OK
9       OK
10      OK
11      OK
12      OK
13      OK
14      OK
15      OK
16      OK
17      OK
18      OK
19      OK
20      OK
21      OK
22      OK
23      OK
24      OK
25      OK
26      OK
27      OK
28      OK
29      OK
30      OK
31      OK
32      OK
33      OK
34      OK
35      OK
36      OK
37      OK
38      OK
39      OK
40      OK
41      OK
42      OK
43      OK
44      OK
45      OK
46      OK
47      OK
48      OK
49      OK
50      OK
51      OK
52      OK
53      OK
54      OK
55      OK
56      OK
57      OK
58      OK
59      OK
60      OK
61      OK
62      OK
63      OK
64      OK
128     OK
192     OK
256     OK
68/68 pass correctness test
bernard@aqua:~/code/csapp/archlab/sim/pipe$ ./benchmark.pl
        ncopy
0       13
1       29      29.00
2       45      22.50
3       57      19.00
4       73      18.25
5       85      17.00
6       101     16.83
7       113     16.14
8       129     16.12
9       141     15.67
10      157     15.70
11      169     15.36
12      185     15.42
13      197     15.15
14      213     15.21
15      225     15.00
16      241     15.06
17      253     14.88
18      269     14.94
19      281     14.79
20      297     14.85
21      309     14.71
22      325     14.77
23      337     14.65
24      353     14.71
25      365     14.60
26      381     14.65
27      393     14.56
28      409     14.61
29      421     14.52
30      437     14.57
31      449     14.48
32      465     14.53
33      477     14.45
34      493     14.50
35      505     14.43
36      521     14.47
37      533     14.41
38      549     14.45
39      561     14.38
40      577     14.43
41      589     14.37
42      605     14.40
43      617     14.35
44      633     14.39
45      645     14.33
46      661     14.37
47      673     14.32
48      689     14.35
49      701     14.31
50      717     14.34
51      729     14.29
52      745     14.33
53      757     14.28
54      773     14.31
55      785     14.27
56      801     14.30
57      813     14.26
58      829     14.29
59      841     14.25
60      857     14.28
61      869     14.25
62      885     14.27
63      897     14.24
64      913     14.27
Average CPE     15.18
Score   0.0/60.0

```

## reference

1. [Lab4 CSAPP: Archlab 高分(56.9/60.0)通过并配好Ubuntu20.04环境](https://zhuanlan.zhihu.com/p/454779772)
2. [CSAPP : Arch Lab 解题报告](https://zhuanlan.zhihu.com/p/36793761)
3. [CSAPP-Lab04 Architecture Lab 深入解析](https://zhuanlan.zhihu.com/p/480380496)