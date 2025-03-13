#include "syscall.h"

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

typedef struct string_t {
	byte *base;
	uint64 len;
} string;

typedef struct slice_t {
	void *base;
	uint64 len;
	uint64 cap;
} slice;

void slice_left(slice * s, uint64 lbytes)
{
	s->base += +lbytes;
	s->len = s->len - lbytes;
	s->cap = s->cap - lbytes;
}

int main()
{
	char msg[] = "Hello\n";
	slice s;

	s.base = msg;
	s.len = sizeof(msg);
	s.cap = sizeof(msg);

	sys_write(1, msg, sizeof(msg));
	slice_left(&s, 2);
	sys_write(1, s.base, s.len);
	return 0;
}
