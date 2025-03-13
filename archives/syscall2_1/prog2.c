#include "u.h"
#include "syscall2.h"

static const char msg[] = "Testing string!\n";
static char retv[64];
uint64 errno;

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
    end = buf-1;
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

void _start()
{
	uint64 res, res2;
	int l;

	res = sys_write(1, msg, 17);
	res2 = errno;

	itoa(res, retv, 64);
	l = mystrlen(retv);
	retv[l] = '\n';
	retv[l+1] = '\0';
	sys_write(1, retv, l+1);

	itoa(res2, retv, 64);
	l = mystrlen(retv);
	retv[l] = '\n';
	retv[l+1] = '\0';
	sys_write(1, retv, l+1);

	sys_exit(60);
}
