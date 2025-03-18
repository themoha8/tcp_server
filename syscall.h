#ifndef SYSCALL_H
#define SYSCALL_H

extern int errno;

enum { map_private = 0x2, map_anonymous = 0x20, map_fixed = 0x10,
	prot_read = 0x1, prot_write = 0x2
};

enum { clock_realtime = 0x0 };

enum { stdin = 0, stdout = 1, stderr = 2 };

struct timespec;

int64 sys_read(uint32 fd, char *buf, uint64 count);
int64 sys_write(uint32 fd, const char *buf, uint64 count);
void *sys_mmap(uintptr addr, uint64 len, uintptr prot, uintptr flags,
			   uintptr fd, uintptr offset);
/*int64 sys_munmap(uintptr addr, uint64 len);*/
void sys_exit(int error_code);
int64 sys_clock_gettime(int32 which_clock, struct timespec *tp);

#endif
