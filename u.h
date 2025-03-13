#ifndef U_H
#define U_H

#define nil (void *)0

enum { stdin = 0, stdout = 1, stderr = 2 };
enum { map_private = 0x2, map_anonymous = 0x20, map_fixed = 0x10,
	prot_read = 0x1, prot_write = 0x2, prot_exec = 0x4
};

typedef char int8;
typedef unsigned char uint8;
typedef unsigned char byte;

typedef short int16;
typedef unsigned short uint16;

typedef int int32;
typedef unsigned int uint32;

typedef long int64;
typedef unsigned long uint64;

typedef float float32;
typedef double float64;

typedef unsigned long uintptr;

#endif
