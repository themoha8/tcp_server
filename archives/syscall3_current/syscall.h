#ifndef SYSCALL_H
#define SYSCALL_H

extern int errno;

int64 sys_write3(uint32 fd, const char *buf, uint64 count);
int64 sys_write6(uint32 fd, const char *buf, uint64 count);
void sys_exit(int error_code);

#endif
