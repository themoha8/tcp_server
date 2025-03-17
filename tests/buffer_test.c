#include "u.h"
#include "builtin.h"
#include "syscall.h"
#include "print.h"
#include "buffer.h"

const char *poem =
	"\nIn the realm of code where logic reigns, \n"
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
	"In the heart of the program, they find their way.\n\nAuthor: anon\n";

void _start(void)
{
	slice s;
	circular_buffer cb = new_circular_buffer(4096);
	uint64 len_poem = c_string_length(poem);
	uint64 len;

	if (cb.buf == nil) {
		sys_write(stderr, "New_circular_buffer failed\n", 27);
		sys_exit(1);
	}
	memcpy(cb.buf, poem, len_poem);
	produce(&cb, len_poem);

	sys_write(stdout, cb.buf, len_poem);
	consume(&cb, len_poem);

	s = remaining_slice(&cb);
	len = put_c_string_in_slice(s, "Bla bla bla\n");
	len += put_c_string_in_slice(slice_left(s, len), "Py py py...\n");
	len += put_int_in_slice(slice_left(s, len), 158);
	len += put_c_string_in_slice(slice_left(s, len), "\n");
	produce(&cb, len);

	print_string(stdout, get_string_from_slice(slice_right(s, len)));
	consume(&cb, len);
	sys_exit(0);
}
