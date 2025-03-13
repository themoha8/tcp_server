#include "u.h"
#include "builtin.h"
#include "syscall.h"
#include "print.h"
#include "slice.h"
#include "error.h"

char msg[] = "Test\n";
char msg2[] = "This is a mmap area!\n";

void _start(void)
{
	int i;
	char *tmp, *p;
	/*slice s;*/

	p = (char *)sys_mmap(0, 4096, prot_read | prot_write, map_private | map_anonymous, -1, 0);

	if (p == nil) {
		sys_write(stderr, "sys_mmap error\n", 16);
		sys_exit(1);
	}
	
	tmp = p;
	for (i = 0; msg2[i] != '\0'; i++) {
		*tmp = msg2[i];
		tmp++;
	}

	print_string(stdout, unsafe_c_string(msg));
	print_string(stdout, unsafe_c_string(p));
	sys_exit(0);
}
