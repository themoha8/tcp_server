#ifndef BUILTIN_H
#define BUILTIN_H

enum { stdin = 0, stdout = 1, stderr = 2 };

typedef struct string_t {
	byte *base;
	uint64 len;
} string;

typedef struct slice_t {
	void *base;
	uint64 len;
	uint64 cap;
} slice;

#define append(vs, v) \
	do { \
		int i, num = 1; \
		if ((vs).len + num > (vs).cap) { \
			(vs) = growslice((vs).base, (vs).len + num, (vs).cap, num, sizeof(v)); \
		} \
		for (i = 0; i < num; ++i) { \
			memcpy((char *)(vs).base + (vs).len*sizeof(v), &(v), sizeof(v)); \
			(vs).len += 1; \
		} \
	} while(0)

/* arena.h */
void *allocate(uint64 size);

#define make(type, len, cap) make_slice(sizeof(type), len, cap)
#define new(type) new_object(type)
#define new_object(type) allocate(sizeof(type))

uint64 copy(slice dst, slice src);
slice make_slice(uint64 type_size, uint64 len, uint64 cap);
slice grow_slice(void *old_ptr, uint64 new_len, uint64 old_cap, uint64 num,
				 uint64 type_size);
string sl_to_str_new_base(slice s);

void panic(const char *msg);

slice get_slice_from_string(string s);
slice unsafe_slice(void *buf, uint64 len);
slice slice_left(slice s, uint64 l_bytes);
slice slice_right(slice s, uint64 r_bytes);
slice slice_left_right(slice s, uint64 l_bytes, uint64 r_bytes);
string get_string_from_slice(slice s);
string unsafe_string(byte * buf, uint64 len);
string unsafe_c_string(char *c_str);
string string_left(string s, uint64 l_bytes);
string string_right(string s, uint64 r_bytes);
string string_left_right(string s, uint64 l_bytes, uint64 r_bytes);
uint64 c_string_length(const char *s);
void *memcpy(void *dst, const void *src, uint64 length);
void *memmove(void *dst, const void *src, uint64 length);
int memequal(const void *dst, const void *src, uint64 length);

uint64 put_c_string_in_slice(slice s, char *c_str);
uint64 put_string_in_slice(slice sl, string s);
uint64 put_int_in_slice(slice s, int x);

#endif
