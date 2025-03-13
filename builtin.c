#include "u.h" /* data types */
#include "error.h" /* assert */
#include "syscall.h" /* sys_write */
#include "builtin.h"

typedef int word;
enum { max_alloc = 1 << 31, wsize = sizeof(word), wmask = wsize - 1 };

void *memcpy(void *dst0, const void *src0, uint64 length)
{
	char *dst = dst0;
	const char *src = src0;
	uint64 t;

	if (length == 0 || dst == src)	/* Nothing to do. */
		return dst;

	if ((uintptr) dst < (uintptr) src) {
		/* Copy forward. */
		t = (uintptr) src;		/* Only need low bits. */
		if ((t | (uintptr) dst) & wmask) {
			/* Try to align operands. This cannot be done
			 * unless the low bits match.
			 */
			if ((t ^ (uintptr) dst) & wmask || length < wsize)
				t = length;
			else
				t = wsize - (t & wmask);

			length -= t;
			do {
				*dst = *src;
				dst++;
				src++;
				t--;
			} while (t);
		}
		/* Copy whole words, then mop up any trailing bytes. */
		t = length / wsize;
		if (t) {
			do {
				*(word *) dst = *(word *) src;
				src += wsize;
				dst += wsize;
				t--;
			} while (t);
		}
		t = length & wmask;
		if (t) {
			do {
				*dst = *src;
				dst++;
				src++;
				t--;
			} while (t);
		}
	} else {
		/*
		 * Copy backwards.  Otherwise essentially the same. 
		 * Alignment works as before, except that it takes
		 * (t&wmask) bytes to align, not wsize-(t&wmask).
		 */
		src += length;
		dst += length;
		t = (uintptr) src;
		if ((t | (uintptr) dst) & wmask) {
			if ((t ^ (uintptr) dst) & wmask || length <= wsize) {
				t = length;
			} else {
				t &= wmask;
			}
			length -= t;
			do {
				dst--;
				src--;
				*dst = *src;
				t--;
			} while (t);
		}
		t = length / wsize;
		if (t) {
			do {
				src -= wsize;
				dst -= wsize;
				*(word *) dst = *(word *) src;
				t--;
			} while (t);
		}
		t = length & wmask;
		if (t) {
			do {
				dst--;
				src--;
				*dst = *src;
				t--;
			} while (t);
		}
	}
	return dst;
}

uint64 c_string_length(const char *s)
{
	int i = 0;

	while (s[i] != '\0')
		i++;

	return i;
}

slice get_slice_from_string(string s)
{
	slice ret;

	ret.base = s.base;
	ret.len = ret.cap = s.len;

	return ret;
}

slice unsafe_slice(void *buf, uint64 len)
{
	slice s;

	s.base = buf;
	s.len = s.cap = len;

	return s;
}

slice slice_left(slice s, uint64 lbytes)
{
	slice ret;

	ret.base = (byte *) s.base + lbytes;
	ret.len = s.len - lbytes;
	ret.cap = s.cap - lbytes;

	return ret;
}

slice slice_right(slice s, uint64 rbytes)
{
	slice ret;

	assert(rbytes <= s.cap);

	ret.base = s.base;
	ret.len = rbytes;
	ret.cap = s.cap;

	return ret;
}

slice slice_left_right(slice s, uint64 lbytes, uint64 rbytes)
{
	slice ret;

	assert(rbytes <= s.cap);

	ret.base = (char *) s.base + lbytes;
	ret.len = rbytes - lbytes;
	ret.cap = s.cap - lbytes;

	return ret;
}

string get_string_from_slice(slice s)
{
	string ret;

	ret.base = s.base;
	ret.len = s.len;

	return ret;
}

string unsafe_string(byte * buf, uint64 len)
{
	string s;

	s.base = buf;
	s.len = len;

	return s;
}

string unsafe_c_string(char *c_str)
{
	return unsafe_string((byte *) c_str, c_string_length(c_str));
}

string string_left(string s, uint64 lbytes)
{
	string ret;

	ret.base = s.base + lbytes;
	ret.len = s.len - lbytes;

	return ret;
}

string string_right(string s, uint64 rbytes)
{
	string ret;

	ret.base = s.base;
	ret.len = rbytes;

	return ret;
}

string string_left_right(string s, uint64 lbytes, uint64 rbytes)
{
	string ret;

	ret.base = s.base + lbytes;
	ret.len = rbytes - lbytes;

	return ret;
}

uint64 copy(slice dst, slice src)
{
	uint64 l;

	l = dst.len > src.len ? src.len : dst.len;

	memcpy(dst.base, src.base, l);
	return l;
}

void *memmove(void *dst, const void *src, uint64 length)
{
	return memcpy(dst, src, length);
}

int memequal(const void *dst, const void *src, uint64 length)
{
	const char *d = dst, *s = src;

	while (length) {
		if (*d != *s)
			return 0;
		length--;
		d++;
		s++;
	}
	return 1;
}

void panic(const char *msg)
{
	sys_write(stderr, msg, c_string_length(msg));
	/* breakpoint for gdb */
	__asm__ __volatile__("int3");
}

/*slice make_slice(uint64 type_size, uint64 len, uint64 cap)
{
	uint64 mem = type_size * cap;
	slice ret;

	if (mem > max_alloc) {
		if (type_size * len > max_alloc) {
			panic("builtin: len out of range\n");
		}
		panic("builtin: cap out of range");
	}

	ret.base = allocate(mem);
	
}*/

