#include "u.h"
#include "builtin.h"
#include "slice.h"

uint64 put_c_string_in_slice(slice s, char *c_str)
{
	return put_string_in_slice(s, unsafe_c_string(c_str));
}

uint64 put_string_in_slice(slice sl, string s)
{
	uint64 l;

	l = sl.len > s.len ? s.len : sl.len;

	memcpy(sl.base, s.base, l);
	return l;
}

uint64 put_int_in_slice(slice s, int x)
{
	int n_digits = 0, rx = 0, i = 0;
	char *buf = s.base;

	if (x == 0) {
		buf[0] = '0';
		return 1;
	}

	/* sign */
	if (x < 0) {
		x = -x;
		buf[i++] = '-';
	}

	while (x > 0) {
		rx = (10 * rx) + (x % 10);
		x /= 10;
		n_digits++;
	}

	while (n_digits > 0) {
		buf[i++] = (rx % 10) + '0';
		rx /= 10;
		n_digits--;
	}

	return i;
}
