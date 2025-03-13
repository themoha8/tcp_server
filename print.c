#include "u.h"
#include "builtin.h"
#include "syscall.h"

void print_string(int stream, string s)
{
	sys_write(stream, (const char *) s.base, s.len);
}
