#ifndef SYSCALL_H
#define SYSCALL_H

extern uint64 errno;

int64 sys_write(int fd, const void *buf, uint64 nbytes);
void sys_exit(int code);

#endif
