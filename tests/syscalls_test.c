#include "u.h"
#include "builtin.h"
#include "syscall.h"

const char *poem =
	"\n\nIn the realm of code where logic reigns, \n"
	"A language born from simple chains, \n"
	"With curly braces and semicolons, \n"
	"The art of C, where thought is woven.\n"
	"Variables dance in memory's space, \n"
	"Integers, floats, each finds its place, \n"
	"Arrays align in a structured array, \n"
	"While pointers guide the data's way.\n"
	"Functions call with a rhythmic grace, \n"
	"Parameters passed in a swift embrace, \n"
	"Return values whisper, 'I’m here to stay,' \n"
	"In the heart of the program, they find their way.\n\nAuthor: ";

void _start(void)
{
	int i, r;
	char *tmp, *p;

	p = (char *) sys_mmap(0, 4096, prot_read | prot_write,
						  map_private | map_anonymous, -1, 0);

	if (p == nil) {
		sys_write(stderr, "sys_mmap error\n", 16);
		sys_exit(1);
	}

	tmp = p;
	for (i = 0; poem[i] != '\0'; i++) {
		*tmp = poem[i];
		tmp++;
	}

	sys_write(1,
			  "Please provide your first and last name, we will use them as the author of the poem: ",
			  85);
	r = sys_read(0, p + i, 4096 - i);
	p[i + r] = '\0';
	sys_write(1, p, i + r);
	sys_exit(0);
}
