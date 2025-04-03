#include "u.h"
#include "builtin.h"
#include "syscall.h"
#include "pool.h"

struct pool_t {
	void *top;
	uint64 n_items;				/* Number of items. */
	new_pool_item_func new_item;	/* Function for creating item. */
};

pool *new_pool(new_pool_item_func new, const error ** perr)
{
	uint64 size = pool_size * sizeof(void *);
	void *stack;
	pool *p;
	const error *err;

	stack = sys_mmap((uintptr) nil, size, prot_read | prot_write, map_private | map_anonymous, -1, 0, &err);

	if (err != nil) {
		if (perr != nil)
			*perr = err;
		return nil;
	}

	/* first item in items is pool itself */
	p = (pool *) ((char *) stack + size - sizeof(pool));
	p->top = p;
	p->n_items = 0;
	p->new_item = new;

	if (perr != nil)
		*perr = nil;
	return p;
}

void *pool_get(pool * p)
{
	void *item;
	void **buf;

	if (p->n_items > 0) {
		buf = p->top;
		item = buf[-p->n_items];
		p->n_items--;
	} else {
		item = p->new_item();
	}

	return item;
}

void pool_put(pool * p, void *item)
{
	void **buf = p->top;
	p->n_items++;
	buf[-p->n_items] = item;
}
