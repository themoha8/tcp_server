#include "u.h"
#include "syscall.h"

static const char msg[] = "Hello\n";

void _start(void)
{
	char output[16];
	int64 r;

	sys_write3(1, msg, sizeof(msg));
	r = sys_write6(1, msg, sizeof(msg));
	output[0] = r + '0';
	output[1] = '\n';
	output[2] = errno + '0';
	output[3] = '\n';
	sys_write3(1, output, 4);
	sys_exit(0);
}