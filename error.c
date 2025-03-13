#include "u.h"
#include "builtin.h"
#include "slice.h"
#include "print.h"
#include "error.h"

void assert_fail(char *expr, char *file, uint64 line)
{
	char buf[1024];
	uint64 n = 0;
	slice s;

	/* file:line: Assertion `expr' failed. */
	s = unsafe_slice(buf, sizeof(buf));

	n += put_c_string_in_slice(s, file);
	n += put_c_string_in_slice(slice_left(s, n), ":");
	n += put_int_in_slice(slice_left(s, n), line);
	n += put_c_string_in_slice(slice_left(s, n), ": Assertion `");
	n += put_c_string_in_slice(slice_left(s, n), expr);
	n += put_c_string_in_slice(slice_left(s, n), "' failed.");
	n += put_c_string_in_slice(slice_left(s, n), "\n");

	print_string(stderr, get_string_from_slice(slice_right(s, n)));
	/* breakpoint for gdb */
	__asm__ __volatile__("int3");
}

/*string handle_err(ferror *fe)
{
	error *err = (error *) fe;
	char buf[512];
	int n;
	slice s;

	s = unsafe_slice(buf, sizeof(buf));
	n = copy(s, get_slice_from_string(err->message));
	buf[n++] = ' ';

	if (err->code != 0) {
		n += slicePutInt(slice_left(s, n), err->code);
	}

	return n;

}

ferror syscall_error(char *msg, uintptr code)
{
	error *err;

	if (code == 0)
		return nil;

	err = newobject(error);
	assert(err != nil);

	err->message = unsafe_c_string(msg);
	err->code = (int) code;
	err->error = handle_err;
	return (ferror) err;
}*/
