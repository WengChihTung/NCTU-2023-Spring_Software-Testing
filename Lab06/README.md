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
```
gcc version 11.3.0
on Ubuntu 22.04


### Heap out-of-bounds read/write
source code:
```
#include <stdlib.h>

int main() {
    int *arr = malloc(sizeof(int) * 5);
    arr[5] = 10;
    free(arr);
    return 0;
}

```
ASan report:
```
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc -o t1 heap.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./t1
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc -fsanitize=address -O1 -g -o t2 heap.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./t2
lab5@DESKTOP-VEKOIJ8:~/Lab06$

```
valgrind report:
```
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc heap.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./a.out
lab5@DESKTOP-VEKOIJ8:~/Lab06$ valgrind ./a.out
==269== Memcheck, a memory error detector
==269== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==269== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==269== Command: ./a.out
==269==
==269== error calling PR_SET_PTRACER, vgdb might block
==269== Invalid write of size 4
==269==    at 0x10918B: main (in /home/lab5/Lab06/a.out)
==269==  Address 0x4a8b054 is 0 bytes after a block of size 20 alloc'd
==269==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==269==    by 0x10917E: main (in /home/lab5/Lab06/a.out)
==269==
==269==
==269== HEAP SUMMARY:
==269==     in use at exit: 0 bytes in 0 blocks
==269==   total heap usage: 1 allocs, 1 frees, 20 bytes allocated
==269==
==269== All heap blocks were freed -- no leaks are possible
==269==
==269== For lists of detected and suppressed errors, rerun with: -s
==269== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
lab5@DESKTOP-VEKOIJ8:~/Lab06$

```
Conclusion: ASan cannot detect, valgrind can.


### Stack out-of-bounds read/write
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
Conclusion: Both ASan and valgrind can detect.


### Global out-of-bounds read/write
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
Conclusion: Both ASan and valgrind can detect.


### Use-after-free
source code:
```
#include <stdlib.h>

int main() {
    int *ptr = malloc(sizeof(int));
    free(ptr);
    *ptr = 10;
    return 0;
}

```
ASan report:
```
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc -o t1 free.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./t1
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc -fsanitize=address -O1 -g -o t2 free.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./t2
lab5@DESKTOP-VEKOIJ8:~/Lab06$

```
valgrind report:
```
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc free.c
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./a.out
lab5@DESKTOP-VEKOIJ8:~/Lab06$ valgrind ./a.out
==204== Memcheck, a memory error detector
==204== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==204== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==204== Command: ./a.out
==204==
==204== error calling PR_SET_PTRACER, vgdb might block
==204== Invalid write of size 4
==204==    at 0x109193: main (in /home/lab5/Lab06/a.out)
==204==  Address 0x4a8b040 is 0 bytes inside a block of size 4 free'd
==204==    at 0x484B27F: free (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==204==    by 0x10918E: main (in /home/lab5/Lab06/a.out)
==204==  Block was alloc'd at
==204==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==204==    by 0x10917E: main (in /home/lab5/Lab06/a.out)
==204==
==204==
==204== HEAP SUMMARY:
==204==     in use at exit: 0 bytes in 0 blocks
==204==   total heap usage: 1 allocs, 1 frees, 4 bytes allocated
==204==
==204== All heap blocks were freed -- no leaks are possible
==204==
==204== For lists of detected and suppressed errors, rerun with: -s
==204== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
lab5@DESKTOP-VEKOIJ8:~/Lab06$

```
Conclusion: ASan cannot detect, valgrind can.


### Use-after-return
source code:
```
#include <stdio.h>

int* get_ptr() {
    int x = 10;
    return &x;
}

int main() {
    int *ptr = get_ptr();
    printf("%d\n", *ptr);
    return 0;
}

```
ASan report:
```
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc -o t1 return.c
return.c: In function ‘get_ptr’:
return.c:5:12: warning: function returns address of local variable [-Wreturn-local-addr]
    5 |     return &x;
      |            ^~
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./t1
Segmentation fault (core dumped)
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc -fsanitize=address -O1 -g -o t2 return.c
return.c: In function ‘get_ptr’:
return.c:5:12: warning: function returns address of local variable [-Wreturn-local-addr]
    5 |     return &x;
      |            ^~
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./t2
AddressSanitizer:DEADLYSIGNAL
=================================================================
==430==ERROR: AddressSanitizer: SEGV on unknown address 0x000000000000 (pc 0x7fccaf5f420b bp 0x000000000001 sp 0x7fffe1f61f70 T0)
==430==The signal is caused by a READ memory access.
==430==Hint: address points to the zero page.
    #0 0x7fccaf5f420b in printf /usr/include/x86_64-linux-gnu/bits/stdio2.h:112
    #1 0x7fccaf5f420b in main /home/lab5/Lab06/return.c:10
    #2 0x7fccae9b9d8f in __libc_start_call_main ../sysdeps/nptl/libc_start_call_main.h:58
    #3 0x7fccae9b9e3f in __libc_start_main_impl ../csu/libc-start.c:392
    #4 0x7fccaf5f4124 in _start (/home/lab5/Lab06/t2+0x1124)

AddressSanitizer can not provide additional info.
SUMMARY: AddressSanitizer: SEGV /usr/include/x86_64-linux-gnu/bits/stdio2.h:112 in printf
==430==ABORTING
lab5@DESKTOP-VEKOIJ8:~/Lab06$

```
valgrind report:
```
lab5@DESKTOP-VEKOIJ8:~/Lab06$ gcc return.c
return.c: In function ‘get_ptr’:
return.c:5:12: warning: function returns address of local variable [-Wreturn-local-addr]
    5 |     return &x;
      |            ^~
lab5@DESKTOP-VEKOIJ8:~/Lab06$ ./a.out
Segmentation fault (core dumped)
lab5@DESKTOP-VEKOIJ8:~/Lab06$ valgrind ./a.out
==256== Memcheck, a memory error detector
==256== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==256== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==256== Command: ./a.out
==256==
==256== error calling PR_SET_PTRACER, vgdb might block
==256== Invalid read of size 4
==256==    at 0x1091C4: main (in /home/lab5/Lab06/a.out)
==256==  Address 0x0 is not stack'd, malloc'd or (recently) free'd
==256==
==256==
==256== Process terminating with default action of signal 11 (SIGSEGV)
==256==  Access not within mapped region at address 0x0
==256==    at 0x1091C4: main (in /home/lab5/Lab06/a.out)
==256==  If you believe this happened as a result of a stack
==256==  overflow in your program's main thread (unlikely but
==256==  possible), you can try to increase the size of the
==256==  main thread stack using the --main-stacksize= flag.
==256==  The main thread stack size used in this run was 8388608.
==256==
==256== HEAP SUMMARY:
==256==     in use at exit: 0 bytes in 0 blocks
==256==   total heap usage: 0 allocs, 0 frees, 0 bytes allocated
==256==
==256== All heap blocks were freed -- no leaks are possible
==256==
==256== For lists of detected and suppressed errors, rerun with: -s
==256== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
Segmentation fault (core dumped)
lab5@DESKTOP-VEKOIJ8:~/Lab06$

```
Conclusion: Both ASan and valgrind can detect.


### Lab6
|                      | Valgrind | ASAN |
|        -------       |    --    |  --  |
| Heap out-of-bounds   |     V    |   X  |
| Stack out-of-bounds  |     V    |   V  |
| Global out-of-bounds |     V    |   V  |
| Use-after-free       |     V    |   X  |
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