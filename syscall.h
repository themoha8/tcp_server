#ifndef SYSCALL_H
#define SYSCALL_H

/* Flags for mmap */
enum { map_private = 0x2, map_anonymous = 0x20, map_fixed = 0x10,
	prot_read = 0x1, prot_write = 0x2, s_setcockopt = 0x36
};

/* Flags for time */
enum { clock_realtime = 0x0 };

/* Flags for read and write */
enum { stdin = 0, stdout = 1, stderr = 2 };

/* Flags and data types for network routines */
typedef uint32 in_addr_t;
typedef uint8 sa_family_t;
typedef uint16 in_port_t;

/* Types */
enum {
	/* Stream socket. */
	sock_stream = 1,
	/* Datagram socket. */
	sock_dgram = 2,
	/* Automatically mark descriptor(s) as non-blocking. */
	sock_nonblock = 4000
};

/* Option flags per-socket. */
enum {
	/* Allow local address & port reuse. */
	so_reuseport = 0x00000200
};

/* Address families. */
enum {
	/* Standardized name for AF_LOCAL. */
	af_unix = 1,
	/* Local to host (pipes, portals). */
	af_local = af_unix,
	/* Internetwork: UDP, TCP, etc. */
	af_inet = 2
};

/* Level number for (get/set)sockopt() to apply to socket itself. */
enum { sol_socket = 0xffff };

#define INADDR_ANY ((in_addr_t)0x00000000)

/* Internet address (a structure for historical reasons). */
struct in_addr {
	in_addr_t s_addr;
};

/* Socket address, internet style. */
struct sockaddr_in {
	/* uint8 sin_len; */
	sa_family_t sin_family;
	in_port_t sin_port;
	struct in_addr sin_addr;
	/* char sin_zero[8]; */
};

/* Structure used by kernel to store most addresses. */
struct sockaddr {
	unsigned char sa_len;
	sa_family_t sa_family;
	char sa_data[14];
};

/* Data types for epoll. */
typedef union epoll_data {
	void *ptr;
	int fd;
	uint32 u32;
	uint64 u64;
} epoll_data_t;

struct epoll_event {
	uint32 events;				/* Epoll events. */
	epoll_data_t data;			/* User data variable. */
};

enum {
	/* Add a file descriptor to the interface. */
	epoll_ctl_add = 1,
	/* Remove a file descriptor from the interface. */
	epoll_ctl_del = 2,
	/* Change file descriptor epoll_event structure. */
	epoll_ctl_mod = 3
};

#define EPOLLINT 1
#define EPOLLOUT 4
#define EPOLLET 1u << 31

struct timespec {
	int64 tv_sec;				/* seconds */
	int64 tv_nsec;				/* and nanoseconds */
};

typedef struct error_t {
	int code;
	const char *msg;
} error;

#define EPERM        1			/* Operation not permitted */
#define ENOENT       2			/* No such file or directory */
#define ESRCH        3			/* No such process */
#define EINTR        4			/* Interrupted system call */
#define EIO      	 5			/* I/O error */
#define ENXIO        6			/* No such device or address */
#define E2BIG        7			/* Argument list too long */
#define ENOEXEC      8			/* Exec format error */
#define EBADF        9			/* Bad file number */
#define ECHILD      10			/* No child processes */
#define EAGAIN      11			/* Try again */
#define ENOMEM      12			/* Out of memory */
#define EACCES      13			/* Permission denied */
#define EFAULT      14			/* Bad address */
#define ENOTBLK     15			/* Block device required */
#define EBUSY       16			/* Device or resource busy */
#define EEXIST      17			/* File exists */
#define EXDEV       18			/* Cross-device link */
#define ENODEV      19			/* No such device */
#define ENOTDIR     20			/* Not a directory */
#define EISDIR      21			/* Is a directory */
#define EINVAL      22			/* Invalid argument */
#define ENFILE      23			/* File table overflow */
#define EMFILE      24			/* Too many open files */
#define ENOTTY      25			/* Not a typewriter */
#define ETXTBSY     26			/* Text file busy */
#define EFBIG       27			/* File too large */
#define ENOSPC      28			/* No space left on device */
#define ESPIPE      29			/* Illegal seek */
#define EROFS       30			/* Read-only file system */
#define EMLINK      31			/* Too many links */
#define EPIPE       32			/* Broken pipe */
#define EDOM        33			/* Math argument out of domain of func */
#define ERANGE      34			/* Math result not representable */

int64 sys_read(uint32 fd, char *buf, uint64 count, error ** err);
int64 sys_write(uint32 fd, const char *buf, uint64 count, error ** err);
error *sys_close(uint32 fd);
void *sys_mmap(uintptr addr, uint64 len, uintptr prot, uintptr flags,
			   uintptr fd, uintptr offset, error ** err);
error *sys_munmap(uintptr addr, uint64 len);
void sys_exit(int error_code);
error *sys_clock_gettime(int which_clock, struct timespec *tp);
int sys_socket(int family, int type, int protocol, error ** err);
error *sys_bind(int sockfd, struct sockaddr *addr, int addrlen);
error *sys_setsockopt(int sockfd, int level, int optname,
					  const void *optval, int optlen);
error *sys_listen(int sockfd, int qlen);
int sys_accept(int sockfd, struct sockaddr *addr, int *addrlen,
			   error ** err);
int sys_accept4(int sockfd, struct sockaddr *addr, int *addrlen, int flags,
				error ** err);
int sys_fork(error ** err);
error *sys_epoll_create(int size);
error *sys_epoll_create1(int flags);
int sys_epoll_wait(int epfd, struct epoll_event *events, int maxevents,
				   int timeout, error ** err);
error *sys_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

#endif
