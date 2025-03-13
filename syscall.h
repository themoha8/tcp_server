#ifndef SYSCALL_H
#define SYSCALL_H

extern int errno;

int64 sys_read(uint32 fd, char *buf, uint64 count);
int64 sys_write(uint32 fd, const char *buf, uint64 count);
void *sys_mmap(uintptr addr, uint64 len, uintptr prot, uintptr flags, uintptr fd, uintptr offset );
/*int64 sys_munmap(uintptr addr, uint64 len);*/
void sys_exit(int error_code);
/*int64 sys_write(int fd, const void *buf, uint64 nbytes);
int64 sys_read(int fd, void *buf, uint64 nbytes);
void sys_exit(int code);
void *sys_mmap(void *addr, uint64 len, int prot, int flags, int fd, int64 offset);
int sys_munmap(void *addr, uint64 len);
*/

#endif
