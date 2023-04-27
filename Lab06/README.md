## Compiler and Version
```
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc -v
Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-linux-gnu/11/lto-wrapper
OFFLOAD_TARGET_NAMES=nvptx-none:amdgcn-amdhsa
OFFLOAD_TARGET_DEFAULT=1
Target: x86_64-linux-gnu
Thread model: posix
Supported LTO compression algorithms: zlib zstd
gcc version 11.3.0 (Ubuntu 11.3.0-1ubuntu1~22.04)
lab5@DESKTOP-VEKOIJ8:~/Lab06$ valgrind --version
valgrind-3.18.1

```
gcc version 11.3.0
valgrind version 3.18.1
on Ubuntu 22.04


### Heap out-of-bounds
source code:
```
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int staticvar;
const int constvar = 0;

int main(void)
{
        int stackvar;
        char buf[200];
        int *p;

        p = malloc(sizeof(int));
        sprintf(buf, "cat /proc/%d/maps", getpid());
        system(buf);

        printf("&staticvar=%p\n", &staticvar);
        printf("&constvar=%p\n", &constvar);
        printf("&stackvar=%p\n", &stackvar);
        printf("p=%p\n", p);
        printf("undefined behaviour: &p[500]=%p\n", &p[500]);
        printf("undefined behaviour: &p[50000000]=%p\n", &p[50000000]);

        p[500] = 999999; //undefined behaviour
        printf("undefined behaviour: p[500]=%d\n", p[500]);
        return 0;
}

```
ASan report:
```
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc -o t1 heap.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./t1
7fd54d410000-7fd54d413000 rw-p 00000000 00:00 0
7fd54d420000-7fd54d448000 r--p 00000000 00:00 478366             /usr/lib/x86_64-linux-gnu/libc.so.6
7fd54d448000-7fd54d5dd000 r-xp 00028000 00:00 478366             /usr/lib/x86_64-linux-gnu/libc.so.6
7fd54d5dd000-7fd54d635000 r--p 001bd000 00:00 478366             /usr/lib/x86_64-linux-gnu/libc.so.6
7fd54d635000-7fd54d639000 r--p 00214000 00:00 478366             /usr/lib/x86_64-linux-gnu/libc.so.6
7fd54d639000-7fd54d63b000 rw-p 00218000 00:00 478366             /usr/lib/x86_64-linux-gnu/libc.so.6
7fd54d63b000-7fd54d648000 rw-p 00000000 00:00 0
7fd54d650000-7fd54d651000 r--p 00000000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7fd54d651000-7fd54d652000 r--p 00001000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7fd54d652000-7fd54d67b000 r-xp 00002000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7fd54d67b000-7fd54d67c000 r-xp 0002b000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7fd54d67c000-7fd54d686000 r--p 0002c000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7fd54d686000-7fd54d687000 r--p 00036000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7fd54d688000-7fd54d68a000 r--p 00037000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7fd54d68a000-7fd54d68c000 rw-p 00039000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7fd54d690000-7fd54d692000 rw-p 00000000 00:00 0
7fd54d697000-7fd54d698000 r--p 00000000 00:00 43320              /home/lab5/Lab06/t1
7fd54d698000-7fd54d699000 r-xp 00001000 00:00 43320              /home/lab5/Lab06/t1
7fd54d699000-7fd54d69a000 r--p 00002000 00:00 43320              /home/lab5/Lab06/t1
7fd54d69a000-7fd54d69b000 r--p 00002000 00:00 43320              /home/lab5/Lab06/t1
7fd54d69b000-7fd54d69c000 rw-p 00003000 00:00 43320              /home/lab5/Lab06/t1
7fffee9df000-7fffeea00000 rw-p 00000000 00:00 0                  [heap]
7ffff5607000-7ffff5e07000 rw-p 00000000 00:00 0                  [stack]
7ffff6434000-7ffff6435000 r-xp 00000000 00:00 0                  [vdso]
&staticvar=0x7fd54d69b014
&constvar=0x7fd54d699008
&stackvar=0x7ffff5e053a4
p=0x7fffee9df2a0
undefined behaviour: &p[500]=0x7fffee9dfa70
undefined behaviour: &p[50000000]=0x7ffffa89b4a0
undefined behaviour: p[500]=999999
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc -fsanitize=address -O1 -g -o t2 heap.c
heap.c: In function ‘main’:
heap.c:16:9: warning: ignoring return value of ‘system’ declared with attribute ‘warn_unused_result’ [-Wunused-result]
   16 |         system(buf);
      |         ^~~~~~~~~~~
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./t2
7fff7000-8fff7000 rw-p 00000000 00:00 0
8fff7000-2008fff7000 ---p 00000000 00:00 0
2008fff7000-10007fff8000 rw-p 00000000 00:00 0
600000000000-602000000000 ---p 00000000 00:00 0
602000000000-602000010000 rw-p 00000000 00:00 0
602000010000-602e00000000 ---p 00000000 00:00 0
602e00000000-602e00010000 rw-p 00000000 00:00 0
602e00010000-603000000000 ---p 00000000 00:00 0
603000000000-603000010000 rw-p 00000000 00:00 0
603000010000-603e00000000 ---p 00000000 00:00 0
603e00000000-603e00010000 rw-p 00000000 00:00 0
603e00010000-607000000000 ---p 00000000 00:00 0
607000000000-607000010000 rw-p 00000000 00:00 0
607000010000-607e00000000 ---p 00000000 00:00 0
607e00000000-607e00010000 rw-p 00000000 00:00 0
607e00010000-60b000000000 ---p 00000000 00:00 0
60b000000000-60b000010000 rw-p 00000000 00:00 0
60b000010000-60be00000000 ---p 00000000 00:00 0
60be00000000-60be00010000 rw-p 00000000 00:00 0
60be00010000-624000000000 ---p 00000000 00:00 0
624000000000-624000010000 rw-p 00000000 00:00 0
624000010000-624e00000000 ---p 00000000 00:00 0
624e00000000-624e00010000 rw-p 00000000 00:00 0
624e00010000-640000000000 ---p 00000000 00:00 0
640000000000-640000003000 rw-p 00000000 00:00 0
7fdfc5c00000-7fdfc5d00000 rw-p 00000000 00:00 0
7fdfc5e00000-7fdfc5f00000 rw-p 00000000 00:00 0
7fdfc5fe0000-7fdfc6200000 rw-p 00000000 00:00 0
7fdfc6300000-7fdfc6400000 rw-p 00000000 00:00 0
7fdfc6480000-7fdfc6481000 rw-p 00000000 00:00 0
7fdfc6490000-7fdfc6491000 rw-p 00000000 00:00 0
7fdfc64a0000-7fdfc64b2000 rw-p 00000000 00:00 0
7fdfc64c0000-7fdfc64c1000 rw-p 00000000 00:00 0
7fdfc64d0000-7fdfc64d1000 rw-p 00000000 00:00 0
7fdfc64e0000-7fdfc64e1000 rw-p 00000000 00:00 0
7fdfc64f0000-7fdfc64f2000 rw-p 00000000 00:00 0
7fdfc6500000-7fdfc6501000 rw-p 00000000 00:00 0
7fdfc6510000-7fdfc851e000 rw-p 00000000 00:00 0
7fdfc8520000-7fdfc8d20000 ---p 00000000 00:00 0
7fdfc8d20000-7fdfc8d28000 rw-p 00000000 00:00 0
7fdfc8d30000-7fdfc8d31000 rw-p 00000000 00:00 0
7fdfc8d40000-7fdfc8d41000 rw-p 00000000 00:00 0
7fdfc8d50000-7fdfc90a2000 rw-p 00000000 00:00 0
7fdfc90b0000-7fdfc90b2000 rw-p 00000000 00:00 0
7fdfc90c0000-7fdfc90c2000 rw-p 00000000 00:00 0
7fdfc90d0000-7fdfc90d1000 rw-p 00000000 00:00 0
7fdfc90e0000-7fdfc90e2000 rw-p 00000000 00:00 0
7fdfc90f0000-7fdfc90f3000 r--p 00000000 00:00 478440             /usr/lib/x86_64-linux-gnu/libgcc_s.so.1
7fdfc90f3000-7fdfc910a000 r-xp 00003000 00:00 478440             /usr/lib/x86_64-linux-gnu/libgcc_s.so.1
7fdfc910a000-7fdfc910e000 r--p 0001a000 00:00 478440             /usr/lib/x86_64-linux-gnu/libgcc_s.so.1
7fdfc910e000-7fdfc910f000 r--p 0001d000 00:00 478440             /usr/lib/x86_64-linux-gnu/libgcc_s.so.1
7fdfc910f000-7fdfc9110000 rw-p 0001e000 00:00 478440             /usr/lib/x86_64-linux-gnu/libgcc_s.so.1
7fdfc9110000-7fdfc911e000 r--p 00000000 00:00 478736             /usr/lib/x86_64-linux-gnu/libm.so.6
7fdfc911e000-7fdfc919a000 r-xp 0000e000 00:00 478736             /usr/lib/x86_64-linux-gnu/libm.so.6
7fdfc919a000-7fdfc91f5000 r--p 0008a000 00:00 478736             /usr/lib/x86_64-linux-gnu/libm.so.6
7fdfc91f5000-7fdfc91f6000 r--p 000e4000 00:00 478736             /usr/lib/x86_64-linux-gnu/libm.so.6
7fdfc91f6000-7fdfc91f7000 rw-p 000e5000 00:00 478736             /usr/lib/x86_64-linux-gnu/libm.so.6
7fdfc9200000-7fdfc9228000 r--p 00000000 00:00 478366             /usr/lib/x86_64-linux-gnu/libc.so.6
7fdfc9228000-7fdfc93bd000 r-xp 00028000 00:00 478366             /usr/lib/x86_64-linux-gnu/libc.so.6
7fdfc93bd000-7fdfc9415000 r--p 001bd000 00:00 478366             /usr/lib/x86_64-linux-gnu/libc.so.6
7fdfc9415000-7fdfc9419000 r--p 00214000 00:00 478366             /usr/lib/x86_64-linux-gnu/libc.so.6
7fdfc9419000-7fdfc941b000 rw-p 00218000 00:00 478366             /usr/lib/x86_64-linux-gnu/libc.so.6
7fdfc941b000-7fdfc9428000 rw-p 00000000 00:00 0
7fdfc9430000-7fdfc9454000 r--p 00000000 00:00 814929             /usr/lib/x86_64-linux-gnu/libasan.so.6.0.0
7fdfc9454000-7fdfc952b000 r-xp 00024000 00:00 814929             /usr/lib/x86_64-linux-gnu/libasan.so.6.0.0
7fdfc952b000-7fdfc955d000 r--p 000fb000 00:00 814929             /usr/lib/x86_64-linux-gnu/libasan.so.6.0.0
7fdfc955d000-7fdfc955e000 ---p 0012d000 00:00 814929             /usr/lib/x86_64-linux-gnu/libasan.so.6.0.0
7fdfc955e000-7fdfc9562000 r--p 0012d000 00:00 814929             /usr/lib/x86_64-linux-gnu/libasan.so.6.0.0
7fdfc9562000-7fdfc9565000 rw-p 00131000 00:00 814929             /usr/lib/x86_64-linux-gnu/libasan.so.6.0.0
7fdfc9565000-7fdfc9e19000 rw-p 00000000 00:00 0
7fdfc9e20000-7fdfc9e21000 r--p 00000000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7fdfc9e21000-7fdfc9e22000 r--p 00001000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7fdfc9e22000-7fdfc9e4b000 r-xp 00002000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7fdfc9e4b000-7fdfc9e4c000 r-xp 0002b000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7fdfc9e4c000-7fdfc9e56000 r--p 0002c000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7fdfc9e56000-7fdfc9e57000 r--p 00036000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7fdfc9e58000-7fdfc9e5a000 r--p 00037000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7fdfc9e5a000-7fdfc9e5c000 rw-p 00039000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7fdfc9e60000-7fdfc9e62000 rw-p 00000000 00:00 0
7fdfc9e6b000-7fdfc9e6c000 r--p 00000000 00:00 43933              /home/lab5/Lab06/t2
7fdfc9e6c000-7fdfc9e6d000 r-xp 00001000 00:00 43933              /home/lab5/Lab06/t2
7fdfc9e6d000-7fdfc9e6e000 r--p 00002000 00:00 43933              /home/lab5/Lab06/t2
7fdfc9e6e000-7fdfc9e6f000 r--p 00002000 00:00 43933              /home/lab5/Lab06/t2
7fdfc9e6f000-7fdfc9e70000 rw-p 00003000 00:00 43933              /home/lab5/Lab06/t2
7fffee11d000-7fffee91d000 rw-p 00000000 00:00 0                  [stack]
7fffeeeb8000-7fffeeeb9000 r-xp 00000000 00:00 0                  [vdso]
&staticvar=0x7fdfc9e6f2e0
&constvar=0x7fdfc9e6d280
&stackvar=0x7fffee91bc90
p=0x602000000010
undefined behaviour: &p[500]=0x6020000007e0
undefined behaviour: &p[50000000]=0x60200bebc210
=================================================================
==318==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x6020000007e0 at pc 0x7fdfc9e6c4d3 bp 0x7fffee91bc60 sp 0x7fffee91bc50
WRITE of size 4 at 0x6020000007e0 thread T0
    #0 0x7fdfc9e6c4d2 in main /home/lab5/Lab06/heap.c:25
    #1 0x7fdfc9229d8f in __libc_start_call_main ../sysdeps/nptl/libc_start_call_main.h:58
    #2 0x7fdfc9229e3f in __libc_start_main_impl ../csu/libc-start.c:392
    #3 0x7fdfc9e6c1e4 in _start (/home/lab5/Lab06/t2+0x11e4)

0x6020000007e0 is located 1996 bytes to the right of 4-byte region [0x602000000010,0x602000000014)
allocated by thread T0 here:
    #0 0x7fdfc94e4867 in __interceptor_malloc ../../../../src/libsanitizer/asan/asan_malloc_linux.cpp:145
    #1 0x7fdfc9e6c341 in main /home/lab5/Lab06/heap.c:14

SUMMARY: AddressSanitizer: heap-buffer-overflow /home/lab5/Lab06/heap.c:25 in main
Shadow bytes around the buggy address:
  0x0c047fff80a0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff80b0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff80c0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff80d0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff80e0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
=>0x0c047fff80f0: fa fa fa fa fa fa fa fa fa fa fa fa[fa]fa fa fa
  0x0c047fff8100: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff8110: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff8120: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff8130: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff8140: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
  Shadow gap:              cc
==318==ABORTING
lab5@DESKTOP-VEKOIJ8:~/Lab06$

```
valgrind report:
```
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc heap.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./a.out
7f6c300d0000-7f6c300d3000 rw-p 00000000 00:00 0
7f6c300e0000-7f6c30108000 r--p 00000000 00:00 478366             /usr/lib/x86_64-linux-gnu/libc.so.6
7f6c30108000-7f6c3029d000 r-xp 00028000 00:00 478366             /usr/lib/x86_64-linux-gnu/libc.so.6
7f6c3029d000-7f6c302f5000 r--p 001bd000 00:00 478366             /usr/lib/x86_64-linux-gnu/libc.so.6
7f6c302f5000-7f6c302f9000 r--p 00214000 00:00 478366             /usr/lib/x86_64-linux-gnu/libc.so.6
7f6c302f9000-7f6c302fb000 rw-p 00218000 00:00 478366             /usr/lib/x86_64-linux-gnu/libc.so.6
7f6c302fb000-7f6c30308000 rw-p 00000000 00:00 0
7f6c30310000-7f6c30311000 r--p 00000000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7f6c30311000-7f6c30312000 r--p 00001000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7f6c30312000-7f6c3033b000 r-xp 00002000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7f6c3033b000-7f6c3033c000 r-xp 0002b000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7f6c3033c000-7f6c30346000 r--p 0002c000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7f6c30346000-7f6c30347000 r--p 00036000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7f6c30348000-7f6c3034a000 r--p 00037000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7f6c3034a000-7f6c3034c000 rw-p 00039000 00:00 477919             /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7f6c30350000-7f6c30352000 rw-p 00000000 00:00 0
7f6c30352000-7f6c30353000 r--p 00000000 00:00 46149              /home/lab5/Lab06/a.out
7f6c30353000-7f6c30354000 r-xp 00001000 00:00 46149              /home/lab5/Lab06/a.out
7f6c30354000-7f6c30355000 r--p 00002000 00:00 46149              /home/lab5/Lab06/a.out
7f6c30355000-7f6c30356000 r--p 00002000 00:00 46149              /home/lab5/Lab06/a.out
7f6c30356000-7f6c30357000 rw-p 00003000 00:00 46149              /home/lab5/Lab06/a.out
7fffdae61000-7fffdae82000 rw-p 00000000 00:00 0                  [heap]
7fffe2aaa000-7fffe32aa000 rw-p 00000000 00:00 0                  [stack]
7fffe32f2000-7fffe32f3000 r-xp 00000000 00:00 0                  [vdso]
&staticvar=0x7f6c30356014
&constvar=0x7f6c30354008
&stackvar=0x7fffe32a86a4
p=0x7fffdae612a0
undefined behaviour: &p[500]=0x7fffdae61a70
undefined behaviour: &p[50000000]=0x7fffe6d1d4a0
undefined behaviour: p[500]=999999
lab5@DESKTOP-VEKOIJ8:~/Lab06$ valgrind ./a.out
==339== Memcheck, a memory error detector
==339== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==339== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==339== Command: ./a.out
==339==
==339== error calling PR_SET_PTRACER, vgdb might block
00108000-00109000 r--p 00000000 00:00 46149                      /home/lab5/Lab06/a.out
00109000-0010a000 r-xp 00001000 00:00 46149                      /home/lab5/Lab06/a.out
0010a000-0010b000 r--p 00002000 00:00 46149                      /home/lab5/Lab06/a.out
0010b000-0010c000 r--p 00002000 00:00 46149                      /home/lab5/Lab06/a.out
0010c000-0010d000 rw-p 00003000 00:00 46149                      /home/lab5/Lab06/a.out
04000000-04002000 r--p 00000000 00:00 477919                     /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
04002000-0402c000 r-xp 00002000 00:00 477919                     /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
0402c000-04037000 r--p 0002c000 00:00 477919                     /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
04038000-0403a000 r--p 00037000 00:00 477919                     /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
0403a000-0403c000 rw-p 00039000 00:00 477919                     /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
0403c000-0403d000 rwxp 00000000 00:00 0
0483c000-0483e000 rw-p 00000000 00:00 0
0483e000-0483f000 r--p 00000000 00:00 865228                     /usr/libexec/valgrind/vgpreload_core-amd64-linux.so
0483f000-04840000 r-xp 00001000 00:00 865228                     /usr/libexec/valgrind/vgpreload_core-amd64-linux.so
04840000-04841000 r--p 00002000 00:00 865228                     /usr/libexec/valgrind/vgpreload_core-amd64-linux.so
04841000-04842000 r--p 00002000 00:00 865228                     /usr/libexec/valgrind/vgpreload_core-amd64-linux.so
04842000-04843000 rw-p 00003000 00:00 865228                     /usr/libexec/valgrind/vgpreload_core-amd64-linux.so
04843000-04848000 r--p 00000000 00:00 865238                     /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so
04848000-04855000 r-xp 00005000 00:00 865238                     /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so
04855000-04858000 r--p 00012000 00:00 865238                     /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so
04858000-04859000 ---p 00015000 00:00 865238                     /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so
04859000-0485a000 r--p 00015000 00:00 865238                     /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so
0485a000-0485b000 rw-p 00016000 00:00 865238                     /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so
04860000-04888000 r--p 00000000 00:00 478366                     /usr/lib/x86_64-linux-gnu/libc.so.6
04888000-04a1d000 r-xp 00028000 00:00 478366                     /usr/lib/x86_64-linux-gnu/libc.so.6
04a1d000-04a75000 r--p 001bd000 00:00 478366                     /usr/lib/x86_64-linux-gnu/libc.so.6
04a75000-04a79000 r--p 00214000 00:00 478366                     /usr/lib/x86_64-linux-gnu/libc.so.6
04a79000-04a7b000 rw-p 00218000 00:00 478366                     /usr/lib/x86_64-linux-gnu/libc.so.6
04a7b000-04a8b000 rw-p 00000000 00:00 0
04a8b000-04e8b000 rwxp 00000000 00:00 0
58000000-58001000 r--p 00000000 00:00 865156                     /usr/libexec/valgrind/memcheck-amd64-linux
58001000-581d7000 r-xp 00001000 00:00 865156                     /usr/libexec/valgrind/memcheck-amd64-linux
581d7000-581d8000 r-xp 001d7000 00:00 865156                     /usr/libexec/valgrind/memcheck-amd64-linux
581d8000-5827c000 r--p 001d8000 00:00 865156                     /usr/libexec/valgrind/memcheck-amd64-linux
5827c000-5827d000 r--p 0027c000 00:00 865156                     /usr/libexec/valgrind/memcheck-amd64-linux
5827d000-58283000 rw-p 0027c000 00:00 865156                     /usr/libexec/valgrind/memcheck-amd64-linux
58283000-59c87000 rw-p 00000000 00:00 0
1002001000-1002bc8000 rwxp 00000000 00:00 0
1002c8c000-1002ca8000 rwxp 00000000 00:00 0
1002ca8000-1002caa000 ---p 00000000 00:00 0
1002caa000-1002daa000 rwxp 00000000 00:00 0
1002daa000-1002dac000 ---p 00000000 00:00 0
1002dac000-1002dad000 rw-s 00000000 00:00 115750                 /tmp/vgdb-pipe-shared-mem-vgdb-339-by-lab5-on-???
1002dad000-10051f9000 rwxp 00000000 00:00 0
100592c000-1005a2c000 rwxp 00000000 00:00 0
1005b8a000-1005d8a000 rwxp 00000000 00:00 0
1005e7f000-1006122000 rwxp 00000000 00:00 0
1ffeffe000-1fff001000 rw-p 00000000 00:00 0
7fffe9d1f000-7fffea51f000 rw-p 00000000 00:00 0                  [stack]
&staticvar=0x10c014
&constvar=0x10a008
&stackvar=0x1ffeffffb4
p=0x4a8b040
undefined behaviour: &p[500]=0x4a8b810
undefined behaviour: &p[50000000]=0x10947240
==339== Invalid write of size 4
==339==    at 0x109319: main (in /home/lab5/Lab06/a.out)
==339==  Address 0x4a8b810 is 832 bytes inside an unallocated block of size 4,193,040 in arena "client"
==339==
==339== Invalid read of size 4
==339==    at 0x10932C: main (in /home/lab5/Lab06/a.out)
==339==  Address 0x4a8b810 is 832 bytes inside an unallocated block of size 4,193,040 in arena "client"
==339==
undefined behaviour: p[500]=999999
==339==
==339== HEAP SUMMARY:
==339==     in use at exit: 4 bytes in 1 blocks
==339==   total heap usage: 2 allocs, 1 frees, 1,028 bytes allocated
==339==
==339== LEAK SUMMARY:
==339==    definitely lost: 4 bytes in 1 blocks
==339==    indirectly lost: 0 bytes in 0 blocks
==339==      possibly lost: 0 bytes in 0 blocks
==339==    still reachable: 0 bytes in 0 blocks
==339==         suppressed: 0 bytes in 0 blocks
==339== Rerun with --leak-check=full to see details of leaked memory
==339==
==339== For lists of detected and suppressed errors, rerun with: -s
==339== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)
lab5@DESKTOP-VEKOIJ8:~/Lab06$

```
Conclusion: Both ASan and valgrind can detect.


### Stack out-of-bounds
source code:
```
#include <stdio.h>

int main() {
    int arr[5] = {1, 2, 3, 4, 5};
    printf("%d\n", arr[10]);
    return 0;
}

```
ASan report:
```
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc -o t1 stack.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./t1
-1725522544
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc -fsanitize=address -O1 -g -o t2 stack.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./t2
=================================================================
==351==ERROR: AddressSanitizer: stack-buffer-overflow on address 0x7fffea764ee8 at pc 0x7f30c1621433 bp 0x7fffea764e90 sp 0x7fffea764e80
READ of size 4 at 0x7fffea764ee8 thread T0
    #0 0x7f30c1621432 in main /home/lab5/Lab06/stack.c:5
    #1 0x7f30c09d9d8f in __libc_start_call_main ../sysdeps/nptl/libc_start_call_main.h:58
    #2 0x7f30c09d9e3f in __libc_start_main_impl ../csu/libc-start.c:392
    #3 0x7f30c1621184 in _start (/home/lab5/Lab06/t2+0x1184)

Address 0x7fffea764ee8 is located in stack of thread T0 at offset 72 in frame
    #0 0x7f30c1621258 in main /home/lab5/Lab06/stack.c:3

  This frame has 1 object(s):
    [32, 52) 'arr' (line 4) <== Memory access at offset 72 overflows this variable
HINT: this may be a false positive if your program uses some custom stack unwind mechanism, swapcontext or vfork
      (longjmp and C++ exceptions *are* supported)
SUMMARY: AddressSanitizer: stack-buffer-overflow /home/lab5/Lab06/stack.c:5 in main
Shadow bytes around the buggy address:
  0x10007d4e4980: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x10007d4e4990: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x10007d4e49a0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x10007d4e49b0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x10007d4e49c0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x10007d4e49d0: 00 00 00 00 f1 f1 f1 f1 00 00 04 f3 f3[f3]f3 f3
  0x10007d4e49e0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x10007d4e49f0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x10007d4e4a00: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x10007d4e4a10: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x10007d4e4a20: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
  Shadow gap:              cc
==351==ABORTING
lab5@DESKTOP-VEKOIJ8:~/Lab06$

```
valgrind report:
```
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc stack.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./a.out
166763920
lab5@DESKTOP-VEKOIJ8:~/Lab06$ valgrind ./a.out
==282== Memcheck, a memory error detector
==282== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==282== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==282== Command: ./a.out
==282==
==282== error calling PR_SET_PTRACER, vgdb might block
76062096
==282==
==282== HEAP SUMMARY:
==282==     in use at exit: 0 bytes in 0 blocks
==282==   total heap usage: 1 allocs, 1 frees, 1,024 bytes allocated
==282==
==282== All heap blocks were freed -- no leaks are possible
==282==
==282== For lists of detected and suppressed errors, rerun with: -s
==282== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
lab5@DESKTOP-VEKOIJ8:~/Lab06$

```
Conclusion: ASan can detect, valgrind cannot.


### Global out-of-bounds
source code:
```
#include <stdio.h>

int arr[5] = {1, 2, 3, 4, 5};

int main() {
    arr[10] = 10;
    return 0;
}

```
ASan report:
```
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc -o t1 global.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./t1
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc -fsanitize=address -O1 -g -o t2 global.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./t2
=================================================================
==377==ERROR: AddressSanitizer: global-buffer-overflow on address 0x7fdfb7600048 at pc 0x7fdfb75fd203 bp 0x7fffcd8960d0 sp 0x7fffcd8960c0
WRITE of size 4 at 0x7fdfb7600048 thread T0
    #0 0x7fdfb75fd202 in main /home/lab5/Lab06/global.c:6
    #1 0x7fdfb69b9d8f in __libc_start_call_main ../sysdeps/nptl/libc_start_call_main.h:58
    #2 0x7fdfb69b9e3f in __libc_start_main_impl ../csu/libc-start.c:392
    #3 0x7fdfb75fd104 in _start (/home/lab5/Lab06/t2+0x1104)

0x7fdfb7600048 is located 20 bytes to the right of global variable 'arr' defined in 'global.c:3:5' (0x7fdfb7600020) of size 20
SUMMARY: AddressSanitizer: global-buffer-overflow /home/lab5/Lab06/global.c:6 in main
Shadow bytes around the buggy address:
  0x0ffc76eb7fb0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0ffc76eb7fc0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0ffc76eb7fd0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0ffc76eb7fe0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0ffc76eb7ff0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x0ffc76eb8000: 00 00 00 00 00 00 04 f9 f9[f9]f9 f9 00 00 00 00
  0x0ffc76eb8010: f9 f9 f9 f9 f9 f9 f9 f9 00 00 00 00 00 00 00 00
  0x0ffc76eb8020: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0ffc76eb8030: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0ffc76eb8040: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0ffc76eb8050: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
  Shadow gap:              cc
==377==ABORTING
lab5@DESKTOP-VEKOIJ8:~/Lab06$

```
valgrind report:
```
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc global.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./a.out
lab5@DESKTOP-VEKOIJ8:~/Lab06$ valgrind ./a.out
==295== Memcheck, a memory error detector
==295== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==295== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==295== Command: ./a.out
==295==
==295== error calling PR_SET_PTRACER, vgdb might block
==295==
==295== HEAP SUMMARY:
==295==     in use at exit: 0 bytes in 0 blocks
==295==   total heap usage: 0 allocs, 0 frees, 0 bytes allocated
==295==
==295== All heap blocks were freed -- no leaks are possible
==295==
==295== For lists of detected and suppressed errors, rerun with: -s
==295== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
lab5@DESKTOP-VEKOIJ8:~/Lab06$

```
Conclusion: ASan can detect, valgrind cannot.


### Use-after-free
source code:
```
#include <stdlib.h>

int main() {
  char *ptr = (char*)malloc(5 * sizeof(char*));
  free(ptr);
  return ptr[3];
}

```
ASan report:
```
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc -o t1 free.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./t1
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc -fsanitize=address -O1 -g -o t2 free.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./t2
=================================================================
==276==ERROR: AddressSanitizer: heap-use-after-free on address 0x604000000013 at pc 0x7f01a587520e bp 0x7ffff63a68e0 sp 0x7ffff63a68d0
READ of size 1 at 0x604000000013 thread T0
    #0 0x7f01a587520d in main /home/lab5/Lab06/free.c:6
    #1 0x7f01a4c39d8f in __libc_start_call_main ../sysdeps/nptl/libc_start_call_main.h:58
    #2 0x7f01a4c39e3f in __libc_start_main_impl ../csu/libc-start.c:392
    #3 0x7f01a5875104 in _start (/home/lab5/Lab06/t2+0x1104)

0x604000000013 is located 3 bytes inside of 40-byte region [0x604000000010,0x604000000038)
freed by thread T0 here:
    #0 0x7f01a4ef4517 in __interceptor_free ../../../../src/libsanitizer/asan/asan_malloc_linux.cpp:127
    #1 0x7f01a58751e2 in main /home/lab5/Lab06/free.c:5

previously allocated by thread T0 here:
    #0 0x7f01a4ef4867 in __interceptor_malloc ../../../../src/libsanitizer/asan/asan_malloc_linux.cpp:145
    #1 0x7f01a58751d7 in main /home/lab5/Lab06/free.c:4

SUMMARY: AddressSanitizer: heap-use-after-free /home/lab5/Lab06/free.c:6 in main
Shadow bytes around the buggy address:
  0x0c087fff7fb0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c087fff7fc0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c087fff7fd0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c087fff7fe0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c087fff7ff0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x0c087fff8000: fa fa[fd]fd fd fd fd fa fa fa fa fa fa fa fa fa
  0x0c087fff8010: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c087fff8020: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c087fff8030: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c087fff8040: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c087fff8050: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
  Shadow gap:              cc
==276==ABORTING
lab5@DESKTOP-VEKOIJ8:~/Lab06$

```
valgrind report:
```
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc free.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./a.out
lab5@DESKTOP-VEKOIJ8:~/Lab06$ valgrind ./a.out
==293== Memcheck, a memory error detector
==293== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==293== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==293== Command: ./a.out
==293==
==293== error calling PR_SET_PTRACER, vgdb might block
==293== Invalid read of size 1
==293==    at 0x109197: main (in /home/lab5/Lab06/a.out)
==293==  Address 0x4a8b043 is 3 bytes inside a block of size 40 free'd
==293==    at 0x484B27F: free (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==293==    by 0x10918E: main (in /home/lab5/Lab06/a.out)
==293==  Block was alloc'd at
==293==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==293==    by 0x10917E: main (in /home/lab5/Lab06/a.out)
==293==
==293==
==293== HEAP SUMMARY:
==293==     in use at exit: 0 bytes in 0 blocks
==293==   total heap usage: 1 allocs, 1 frees, 40 bytes allocated
==293==
==293== All heap blocks were freed -- no leaks are possible
==293==
==293== For lists of detected and suppressed errors, rerun with: -s
==293== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
lab5@DESKTOP-VEKOIJ8:~/Lab06$

```
Conclusion: Both ASan and valgrind can detect.


### Use-after-return
source code:
```
int *ptr;
__attribute__((noinline))
void FunctionThatEscapesLocalObject() {
  int local[100];
  ptr = &local[0];
}

int main(int argc, char **argv) {
  FunctionThatEscapesLocalObject();
  return ptr[argc];
}

```
ASan report:
```
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc -o t1 return.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./t1
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc -fsanitize=address -O1 -g -o t2 return.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ASAN_OPTIONS=detect_stack_use_after_return=1 ./t2
=================================================================
==167==ERROR: AddressSanitizer: stack-use-after-return on address 0x7f3302e69034 at pc 0x7f3306fe4385 bp 0x7ffffe2a9880 sp 0x7ffffe2a9870
READ of size 4 at 0x7f3302e69034 thread T0
    #0 0x7f3306fe4384 in main /home/lab5/Lab06/return.c:10
    #1 0x7f33063a9d8f in __libc_start_call_main ../sysdeps/nptl/libc_start_call_main.h:58
    #2 0x7f33063a9e3f in __libc_start_main_impl ../csu/libc-start.c:392
    #3 0x7f3306fe4144 in _start (/home/lab5/Lab06/t2+0x1144)

Address 0x7f3302e69034 is located in stack of thread T0 at offset 52 in frame
    #0 0x7f3306fe4218 in FunctionThatEscapesLocalObject /home/lab5/Lab06/return.c:3

  This frame has 1 object(s):
    [48, 448) 'local' (line 4) <== Memory access at offset 52 is inside this variable
HINT: this may be a false positive if your program uses some custom stack unwind mechanism, swapcontext or vfork
      (longjmp and C++ exceptions *are* supported)
SUMMARY: AddressSanitizer: stack-use-after-return /home/lab5/Lab06/return.c:10 in main
Shadow bytes around the buggy address:
  0x0fe6e05c51b0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0fe6e05c51c0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0fe6e05c51d0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0fe6e05c51e0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0fe6e05c51f0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x0fe6e05c5200: f5 f5 f5 f5 f5 f5[f5]f5 f5 f5 f5 f5 f5 f5 f5 f5
  0x0fe6e05c5210: f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5
  0x0fe6e05c5220: f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5
  0x0fe6e05c5230: f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5
  0x0fe6e05c5240: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0fe6e05c5250: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
  Shadow gap:              cc
==167==ABORTING
lab5@DESKTOP-VEKOIJ8:~/Lab06$

```
valgrind report:
```
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc return.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./a.out
lab5@DESKTOP-VEKOIJ8:~/Lab06$ valgrind ./a.out
==226== Memcheck, a memory error detector
==226== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==226== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==226== Command: ./a.out
==226==
==226== error calling PR_SET_PTRACER, vgdb might block
==226== Invalid read of size 4
==226==    at 0x1091BC: main (in /home/lab5/Lab06/a.out)
==226==  Address 0x1ffefffed4 is on thread 1's stack
==226==  428 bytes below stack pointer
==226==
==226==
==226== HEAP SUMMARY:
==226==     in use at exit: 0 bytes in 0 blocks
==226==   total heap usage: 0 allocs, 0 frees, 0 bytes allocated
==226==
==226== All heap blocks were freed -- no leaks are possible
==226==
==226== For lists of detected and suppressed errors, rerun with: -s
==226== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
lab5@DESKTOP-VEKOIJ8:~/Lab06$

```
Conclusion: Both ASan and valgrind can detect.


### Lab6
|                      | Valgrind | ASAN |
|        -------       |    --    |  --  |
| Heap out-of-bounds   |     V    |   V  |
| Stack out-of-bounds  |     X    |   V  |
| Global out-of-bounds |     X    |   V  |
| Use-after-free       |     V    |   V  |
| Use-after-return     |     V    |   V  |


## Stack buffer overflow
source code:
```
#include <stdio.h>
#include <string.h>

void foo(char* str) {
    char buffer[4];
    strcpy(buffer, str);
    printf("buffer: %s\n", buffer);
}

int main() {
    char* str = "0123456789ABCDEF";
    foo(str);
    return 0;
}

```
ASan report:
```
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc -o t1 buffer.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./t1
buffer: 0123456789ABCDEF
*** stack smashing detected ***: terminated
Aborted (core dumped)
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc -fsanitize=address -O1 -g -o t2 buffer.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./t2
*** buffer overflow detected ***: terminated
Aborted (core dumped)
lab5@DESKTOP-VEKOIJ8:~/Lab06$

```
Conclusion: ASan can detect.