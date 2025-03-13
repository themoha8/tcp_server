#include "u.h"
#include "syscall.h"

static const char msg[] = "Testing string!\n";
static char retv[64];

void itoa(int num, char *buf, int size)
{
	char *start, *end;

	if (size > 0)
		/* '\0' */
		size--;
	else
		return;

	if (num == 0) {
		*buf = '0';
		buf++;
		*buf = '\0';
		return;
	}

	if (num < 0 && size > 0) {
		*buf++ = '-';
		num = -num;
		size--;
	}

	start = buf;

	while (num > 0 && size > 0) {
		*buf++ = (num % 10) + '0';
		num /= 10;
		size--;
	}

	*buf = '\0';
	end = buf - 1;
	while (start < end) {
		char temp = *start;
		*start++ = *end;
		*end-- = temp;
	}
}

int mystrlen(const char *s)
{
	int i = 0;

	while (s[i] != '\0')
		i++;

	return i;
}


// static const unsigned long mask01 = 0x0101010101010101;
// static const unsigned long mask80 = 0x8080808080808080;

// #define LONGPTR_MASK (sizeof(long) - 1)
/*
 * Helper macro to return string length if we caught the zero
 * byte.
 */
/*
#define testbyte(x)                                     \
    do                                                  \
    {                                                   \
        if(p[x] == '\0')                                \
            return ((uintptr)p - (uintptr)str + x); \
    } while(0)
*/
// uint64 mystrlen2(const char* str)
// {
//     const char* p;
//     const unsigned long* lp;

//     /* Skip the first few bytes until we have an aligned p */
//     for(p = str; (uintptr)p & LONGPTR_MASK; p++)
//     {
//         if(*p == '\0')
//         {
//             return ((uintptr)p - (uintptr)str);
//         }
//     }

//     /* Scan the rest of the string using word sized operation */
//     // Cast to void to prevent alignment warning
//     for(lp = (const uintptr *)(const void *)p;; lp++)
//     {
//         if((*lp - mask01) & mask80)
//         {
//             p = (const char*)(lp);
//             testbyte(0);
//             testbyte(1);
//             testbyte(2);
//             testbyte(3);
// #if(LONG_BIT >= 64)
//             testbyte(4);
//             testbyte(5);
//             testbyte(6);
//             testbyte(7);
// #endif
//         }
//     }

	/* NOTREACHED */
	// return (0);
// }

int main()
{
	// syscall6(1, 1, (uintptr)msg, 17, 0, 0, 0);
	// int res = sys_write(1, msg, 17);

	uint64 res, res2;
	int l;

	res = sys_write(1, msg, 17);
	res2 = errno;

	itoa(res, retv, 64);
	l = mystrlen(retv);
	retv[l] = '\n';
	retv[l + 1] = '\0';
	sys_write(1, retv, l + 1);

	itoa(res2, retv, 64);
	l = mystrlen(retv);
	retv[l] = '\n';
	retv[l + 1] = '\0';
	sys_write(1, retv, l + 1);

	return 0;
}
