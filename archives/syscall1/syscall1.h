#ifndef SYSCALL_H
#define SYSCALL_H

typedef struct syscall_t {
	uintptr r1;
	uintptr r2;
	uintptr errno;
} syscall_result;

syscall_result sys_write(int fd, const void *buf, uint64 nbytes);
void sys_exit(int code);

// syscall_result syscall3(int trap, uintptr a1, uintptr a2, uintptr a3)
// {
//  return syscall6(trap, a1, a2, a3, 0, 0, 0);
// }
// int64 write(int fd, void *buf, uint64 nbytes, error *perr);

#endif
