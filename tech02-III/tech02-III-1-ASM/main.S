#include <sys/syscall.h>
.intel_syntax noprefix

.global _start
.text
_start:               // Allocate 16 bytes on stack. Can't do less due to stack alignment
  sub  rsp, 16        // Later we will store input chars in allocated space
read_and_write:
  mov  rax, SYS_read
  mov  rdi, 0         // STDOUT_FILENO
  mov  rsi, rsp
  mov  rdx, 1
  syscall             // Read 1 byte from stdin
  cmp  rax, 0   
  jl   exit_1         // Exit with code 1 if return code is < 0 (read returned error)
  cmp  rax, 0
  je   exit_0         // Exit with code 0 if return code is 0 (input has stopped)
  mov  rax, SYS_write
  mov  rdi, 1         // STDIN_FILENO
  mov  rsi, rsp
  mov  rdx, 1
  syscall             // Write the same byte to stdout
  cmp  rax, 0
  jl   exit_2         // Exit with code 2 if return code is < 0 (write returned error)
  jmp  read_and_write // Loop otherwise
exit_0:
  add  rsp, 16        // Don't forget to free stack
  mov  rax, SYS_exit
  mov  rdi, 0
  syscall
exit_1:
  add  rsp, 16
  mov  rax, SYS_exit
  mov  rdi, 1
  syscall
exit_2:
  add  rsp, 16
  mov  rax, SYS_exit
  mov  rdi, 2
  syscall