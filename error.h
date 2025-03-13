#ifndef ERROR_H
#define ERROR_H

void assert_fail(char *expr, char *file, uint64 line);

#define assert(expr) \
	do { \
		if (!(expr)) { \
			assert_fail(#expr, __FILE__, __LINE__); \
		} \
	} while (0)

#endif
