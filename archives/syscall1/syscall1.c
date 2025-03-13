#include "u.h"
#include "syscall1.h"

enum { s_write = 1, s_exit = 60 };

syscall_result syscall3(int trap, uintptr a1, uintptr a2, uintptr a3)
{
	syscall_result r;

  __asm__("syscall\n\t" "cmpq $-4095, %%rax\n\t" "jbe ok\n\t" "movq	$-1, %0\n\t" "movq	$0, %1\n\t" "movq	%%rax, %2\n\t" "jmp quit\n\t" "ok:\n\t" "movq	%%rax, %0\n\t" "movq	%%rdx, %1\n\t" "movq	$0, %2\n\t" "quit:\n\t":
"=m"(r.r1), "=m"(r.r2),
"=m"(r.errno)
  :
"a"(trap), "D"(a1), "S"(a2), "d"(a3)
		);

	return r;
}

void sys_exit(int code)
{
	syscall3(s_exit, code, 0, 0);
}

syscall_result sys_write(int fd, const void *buf, uint64 nbytes)
{
	syscall_result r = syscall3(s_write, fd, (uintptr) buf, nbytes);
	return r;
}

// syscall_result syscall6(int trap, uintptr a1, uintptr a2, uintptr a3,
//                      uintptr a4, uintptr a5, uintptr a6)
// {
//  syscall_result r;

//  __asm__ __volatile__ (
//      "movq   %7, %%r10\n\t"
//      "movq   %8, %%r8\n\t"
//      "movq   %9, %%r9\n\t"
//      "syscall\n\t"
//      "jnc 0f\n\t"
//      "movq   $-1, %0\n\t"
//      "movq   $0, %1\n\t"
//      "movq   %%rax, %2\n\t"
//      "jmp 1f\n\t"
//      "0:\n\t"
//      "movq   %%rax, %0\n\t"
//      "movq   %%rdx, %1\n\t"
//      "movq   $0, %2\n\t"
//      "1:\n\t"
// :
//      "=m" (r.r1), "=m" (r.r2), "=m" (r.errno)
// :
//      "a" (trap), "D" (a1), "S" (a2), "d" (a3), "m" (a4), "m" (a5), "m" (a6)
// :
//      "rcx", "r8", "r9", "r10", "r11"
//      );

//  return r;
// }
