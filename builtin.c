#include "u.h"					/* data types */
#include "error.h"				/* assert */
#include "syscall.h"			/* sys_write */
#include "arena.h"				/* allocate */
#include "builtin.h"

typedef int word;

static const uint64 max_alloc = 1 << 31;

enum { wsize = sizeof(word), wmask = wsize - 1 };

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

slice make_slice(uint64 type_size, uint64 len, uint64 cap)
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
	ret.len = len;
	ret.cap = cap;

	return ret;
}

static uint64 new_slice_cap(uint64 new_len, uint64 old_cap)
{
	uint64 new_cap = old_cap;
	uint64 threshold = 256;
	uint64 double_cap = new_cap + new_cap;

	if (new_len > double_cap)
		return new_len;

	if (old_cap < threshold)
		return double_cap;

	while (1) {
		uint64 old_new_cap = new_cap;

		/* Transition from growing 2x for small slices
		 * to growing 1.25x for large slices. This formula
		 * gives a smooth-ish transition between the two.
		 */
		new_cap += (new_cap + 3 * threshold) >> 2;

		/* check overflow */
		if (new_cap < old_new_cap)
			return new_len;

		if (new_cap >= new_len)
			return new_cap;
	}
}

static uint64 round_up_size(uint64 size)
{
	int page_size = 1 << 12;
	return size + (size & (page_size - 1));
}

slice grow_slice(void *old_ptr, uint64 new_len, uint64 old_cap, uint64 num,
				 uint64 type_size)
{
	uint64 new_cap = new_slice_cap(new_len, old_cap);
	uint64 old_len = new_len - num;
	uintptr len_mem, cap_mem;
	void *p;
	slice ret;

	len_mem = old_len * type_size;
	cap_mem = new_cap * type_size;
	cap_mem = round_up_size(cap_mem);
	new_cap = cap_mem / type_size;
	cap_mem = new_cap * type_size;

	if (cap_mem > max_alloc)
		panic("grow_slice: len out of range\n");

	p = allocate(cap_mem);
	memmove(p, old_ptr, len_mem);

	ret.base = p;
	ret.len = new_len;
	ret.cap = new_cap;

	return ret;
}

string sl_to_str_new_base(slice s)
{
	byte *mem;
	string ret;

	mem = allocate(s.len);
	assert(mem != nil);

	ret.base = mem;
	ret.len = s.len;

	copy(get_slice_from_string(ret), s);
	return ret;
}
