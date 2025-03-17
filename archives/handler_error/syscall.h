#ifndef SYSCALL_H
#define SYSCALL_H

enum { map_private = 0x2, map_anonymous = 0x20, map_fixed = 0x10,
	prot_read = 0x1, prot_write = 0x2
};

int64 sys_read(uint32 fd, char *buf, uint64 count, error **perr);
int64 sys_write(uint32 fd, const char *buf, uint64 count, error **perr);
void *sys_mmap(uintptr addr, uint64 len, uintptr prot, uintptr flags,
			   uintptr fd, uintptr offset, error **perr);
void sys_exit(int error_code);

#endif
