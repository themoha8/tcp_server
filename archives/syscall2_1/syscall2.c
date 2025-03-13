#include "u.h"
#include "syscall2.h"

enum { s_write = 1, s_exit = 60 };

uint64 syscall3(int trap, uintptr a1, uintptr a2, uintptr a3)
{
	uint64 r;

  __asm__("syscall\n\t" "cmpq $-4095, %%rax\n\t" "jbe ok\n\t" "movq	%%rax, %0\n\t" "movq	$-1, %1\n\t" "jmp quit\n\t" "ok:\n\t" "movq	$0, %0\n\t" "movq	%%rax, %1\n\t" "quit:\n\t":
"=m"(errno),
"=m"(r)
  :
"a"(trap), "D"(a1), "S"(a2), "d"(a3)
		);

	return r;
}

int64 sys_write(int fd, const void *buf, uint64 nbytes)
{
	return syscall3(s_write, fd, (uintptr) buf, nbytes);
}

void sys_exit(int code)
{
	syscall3(s_exit, code, 0, 0);
}
