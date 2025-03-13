#ifndef SYSCALL_H
#define SYSCALL_H
#include "u.h"

extern uint64 errno;

int64 sys_write(int fd, const void *buf, uint64 nbytes);
int64 sys_read(int fd, void *buf, uint64 nbytes);
void sys_exit(int code);

#endif
