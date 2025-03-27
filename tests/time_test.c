#include "u.h"
#include "builtin.h"
#include "time.h"
#include "syscall.h"

void _start(void)
{
	struct timespec tp;
	struct tm tm_time;
	slice s;
	uint64 len;
	error *err;

	err = sys_clock_gettime(clock_realtime, &tp);
	if (err != nil) {
		fmt_fprint(stderr, "sys_clock_gettime: %s\n", err->msg);
		sys_exit(1);
	}

	tm_time = time_to_tm(tp.tv_sec);
	s = make(char, 512, 512);

	len = put_c_string_in_slice(s, "Unix time: ");
	len += put_int_in_slice(slice_left(s, len), tp.tv_sec);

	len += put_c_string_in_slice(slice_left(s, len), "\n\nStruct tm: \n");
	len += put_c_string_in_slice(slice_left(s, len), "tm_sec: ");
	len += put_int_in_slice(slice_left(s, len), tm_time.tm_sec);

	len += put_c_string_in_slice(slice_left(s, len), "\ntm_min: ");
	len += put_int_in_slice(slice_left(s, len), tm_time.tm_min);

	len += put_c_string_in_slice(slice_left(s, len), "\ntm_hour: ");
	len += put_int_in_slice(slice_left(s, len), tm_time.tm_hour);

	len += put_c_string_in_slice(slice_left(s, len), "\ntm_mday: ");
	len += put_int_in_slice(slice_left(s, len), tm_time.tm_mday);

	len += put_c_string_in_slice(slice_left(s, len), "\ntm_mon: ");
	len += put_int_in_slice(slice_left(s, len), tm_time.tm_mon);

	len += put_c_string_in_slice(slice_left(s, len), "\ntm_year: ");
	len += put_int_in_slice(slice_left(s, len), tm_time.tm_year);

	len += put_c_string_in_slice(slice_left(s, len), "\ntm_wday: ");
	len += put_int_in_slice(slice_left(s, len), tm_time.tm_wday);

	len += put_c_string_in_slice(slice_left(s, len), "\ntm_yday: ");
	len += put_int_in_slice(slice_left(s, len), tm_time.tm_yday);

	len += put_c_string_in_slice(slice_left(s, len), "\ntm_isdst: ");
	len += put_int_in_slice(slice_left(s, len), tm_time.tm_isdst);

	len += put_c_string_in_slice(slice_left(s, len), "\ntm_gmtoff: ");
	len += put_int_in_slice(slice_left(s, len), tm_time.tm_gmtoff);

	len += put_c_string_in_slice(slice_left(s, len), "\ntm_zone: ");
	len += put_c_string_in_slice(slice_left(s, len), tm_time.tm_zone);

	len += put_c_string_in_slice(slice_left(s, len), "\n\n");

	len += put_c_string_in_slice(slice_left(s, len),
							  "Time in RF822 format: ");
	len += put_tm_rfc822_in_slice(slice_left(s, len), &tm_time);
	len += put_c_string_in_slice(slice_left(s, len),
							  "\n\nTime in tm2 format: ");
	len += put_tm_in_slice(slice_left(s, len), &tm_time);
	len += put_c_string_in_slice(slice_left(s, len), "\n");
	print_string(stdout, get_string_from_slice(slice_right(s, len)));
	sys_exit(0);
}
