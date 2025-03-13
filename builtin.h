#ifndef BUILTIN_H
#define BUILTIN_H

typedef struct string_t {
	byte *base;
	uint64 len;
} string;

typedef struct slice_t {
	void *base;
	uint64 len;
	uint64 cap;
} slice;

/*typedef struct ferror_t *ferror;*/

typedef struct ferror_t {
	string (*handle_error)(struct ferror_t *);
} ferror;

typedef struct error_t {
	string (*handle_error)(ferror*);
	string message;
	int code;
} error;

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

#define make(type, len, cap) make_slice(sizeof(type), len, cap)
#define new(type) new_object(type)
#define new_object(type) allocate(sizeof(type))

uint64 copy(slice dst, slice src);
/*slice make_slice(uint64 type_size, uint64 len, uint64 cap);*/

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

#endif
