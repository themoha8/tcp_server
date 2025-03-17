#include "u.h"
#include "builtin.h"
#include "print.h"
#include "error.h"
#include "syscall.h"

void assert_fail(char *expr, char *file, uint64 line)
{
	char buf[1024];
	uint64 n = 0;
	slice s;

	/* file:line: Assertion 'expr' failed. */
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

string handle_err(error *e)
{
	char buf[512];
	int n;
	slice s;

	s = unsafe_slice(buf, sizeof(buf));
	n = copy(s, get_slice_from_string(e->message));
	buf[n++] = ' ';

	if (e->code != 0) {
		n += put_int_in_slice(slice_left(s, n), e->code);
	}

	n += put_c_string_in_slice(slice_left(s, n), "\n");
	
	return sl_to_str_new_base(slice_right(s, n));
}

error *syscall_error(char *msg, uintptr code)
{
	error *err;

	if (code == 0)
		return nil;

	err = new_object(error);
	assert(err != nil);

	err->message = unsafe_c_string(msg);
	err->code = (int) code;
	err->handler = handle_err;
	return err;
}

void fatal_error(const char *msg, error *err)
{
	sys_write(2, msg, c_string_length(msg), nil);

	if (err != nil)
		print_string(2, err->handler(err));
	else
		sys_write(2, "\n", 1, nil);

	sys_exit(1);
}
