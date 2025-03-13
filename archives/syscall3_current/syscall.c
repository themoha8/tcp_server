#include "u.h"
#include "syscall.h"

int errno;

enum { s_write = 0x1, s_exit = 0x3c };

uintptr syscall6(int trap, uintptr a1, uintptr a2, uintptr a3, uintptr a4, uintptr a5, uintptr a6)
{
	uintptr r;

	__asm__ __volatile__ (
		"movq	%6, %%r10\n\t"
	    "movq	%7, %%r8\n\t"
	    "movq	%8, %%r9\n\t"
		"syscall\n\t"
		"cmpq	$-4095, %%rax\n\t"
		"jbe 	0f\n\t"
		"movq	%%rax, %1\n\t"
		"movq	$-1, %%rax\n\t"
		"jmp	1f\n\t"
		"0:\n\t"
		"movl 	$0, %1\n\t"
		"1:\n\t"
:		"=a" (r), "=m" (errno)
:		"a" (trap), "D" (a1), "S" (a2), "d" (a3), "m" (a4), "m" (a5), "m" (a6)
:		"r8", "r9", "r10"
		);

	return r;
}

uintptr syscall3(int trap, uintptr a1, uintptr a2, uintptr a3)
{
	uintptr r;

	__asm__ __volatile__ (
		"syscall\n\t"
		"cmpq	$-4095, %%rax\n\t"
		"jbe 	0f\n\t"
		"movq	%%rax, %1\n\t"
		"movq	$-1, %%rax\n\t"
		"jmp	1f\n\t"
		"0:\n\t"
		"movl 	$0, %1\n\t"
		"1:\n\t"
:		"=a" (r), "=m" (errno)
:		"a" (trap), "D" (a1), "S" (a2), "d" (a3)
:
		);

	return r;
}

int64 sys_write3(uint32 fd, const char *buf, uint64 count)
{
	return syscall3(s_write, fd, (uintptr) buf, count);
}

int64 sys_write6(uint32 fd, const char *buf, uint64 count)
{
	return syscall6(s_write, fd, (uintptr) buf, count, 0, 0, 0);
}

void sys_exit(int error_code)
{
	syscall3(s_exit, error_code, 0, 0);
}
