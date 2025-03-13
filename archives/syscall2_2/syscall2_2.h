#ifndef SYSCALL_H
#define SYSCALL_H

extern uint64 errno;

uint64 syscall3(int trap, uintptr a1, uintptr a2, uintptr a3);

#endif
