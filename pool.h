#ifndef POOL_H
#define POOL_H

enum { pool_size = 1024 };

typedef void *(*new_pool_item_func) (void);
typedef struct pool_t pool;

pool *new_pool(new_pool_item_func new, const error ** perr);
void *pool_get(pool * p);
void pool_put(pool * p, void *item);

#endif
