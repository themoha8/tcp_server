#ifndef ERROR_H
#define ERROR_H

extern int errno;

typedef struct error_t {
    string (*handler)(struct error_t *e);
    string message;
    int code;
} error;

void assert_fail(char *expr, char *file, uint64 line);

#define assert(expr) \
	do { \
		if (!(expr)) { \
			assert_fail(#expr, __FILE__, __LINE__); \
		} \
	} while (0)

error *syscall_error(char *msg, uintptr code);
void fatal_error(const char *msg, error *err);
#endif
