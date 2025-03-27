#include "u.h"					/* data types */
#include "syscall.h"			/* mmap */
#include "builtin.h"			/* fmt_fprint */
#include "arena.h"

typedef struct arena_t {
	void *base;
	uint64 in_use;
	uint64 allocated;
} arena;

enum { page_size = 4096 };

static arena global_arena;

static error *extend_arena_by(arena * a, uint64 size)
{
	error *err;
	void *addr;
	int flags;
	size = ((size - 1) / page_size + 1) * page_size;

	if (a->base == nil) {
		addr = nil;
		flags = 0;
	} else {
		addr = (char *) a->base + a->allocated;
		flags = map_fixed;
	}

	addr = sys_mmap((uintptr) addr, size, prot_read | prot_write,
					map_private | map_anonymous | flags, -1, 0, &err);

	if (err != nil)
		return err;

	if (a->base == nil)
		a->base = addr;

	a->allocated += size;

	return nil;
}

static void *allocate_from(arena * a, uint64 size)
{
	error *err;
	char *start;

	if (a->in_use + size > a->allocated) {
		err = extend_arena_by(a, size);
		if (err != nil) {
			fmt_fprint(stderr, "Failed to initialize new arena: %s\n",
					   err->msg);
			sys_exit(1);
		}
	}
	start = (char *) a->base + a->in_use;
	a->in_use += size;

	return start;
}

void *allocate(uint64 size)
{
	return allocate_from(&global_arena, size);
}
