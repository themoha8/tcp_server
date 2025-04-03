/* Echo server */
#include "u.h"
#include "builtin.h"
#include "syscall.h"
#include "pool.h"
#include "arena.h"

enum { max_events = 128,
	max_buf = 512,
	/* Client sockets is started with 5. */
	max_sess_arr = pool_size + 2
};

typedef struct session_t {
	int fd;
	int buf_used;
	char buf[max_buf];
} session;

typedef struct server_str_t {
	int ls;
	session *sess_arr[max_sess_arr];
} server_str;

static uint16 swap_bytes_in_port(uint16 port)
{
	return (port << 8) | ((port & 0xff00) >> 8);
}

static session *new_session(void)
{
	session *c;

	c = new(session);
	c->fd = -1;
	c->buf_used = 0;
	return c;
}

static void session_write_if_lf(session * sess)
{
	int i, pos = -1;

	for (i = 0; i < sess->buf_used; i++)
		if (sess->buf[i] == '\n') {
			pos = i;
			break;
		}

	if (pos == -1)
		return;

	sys_write(sess->fd, sess->buf, pos + 1, nil);
	sess->buf_used -= pos + 1;
	memmove(sess->buf, sess->buf + pos + 1, sess->buf_used);
}

static void session_close(session * sess, server_str * serv, pool * sess_pool)
{
	sys_close(sess->fd);
	sess->fd = -1;
	serv->sess_arr[sess->fd] = nil;
	sess->buf_used = 0;
	pool_put(sess_pool, sess);

}

static int sesison_do_read(session * sess)
{
	const error *err;
	int n, bufp = sess->buf_used;

	for (;;) {
		n = sys_read(sess->fd, sess->buf + bufp, max_buf - bufp, &err);
		if (err != nil) {
			if (err->code == EAGAIN)
				break;
			else if (err->code == EINTR)
				continue;
			else {
				fmt_fprint(stderr, "sys_read failed: %s\n", err->msg);
				return 0;
			}
		}
		if (n == 0)
			return 0;

		sess->buf_used += n;
		if (sess->buf_used >= max_buf) {
			sys_write(sess->fd, "Line too long! Good bye...\n", 27, nil);
			return 0;
		}
	}
	return 1;
}

/* =========== server =========== */

static int server_init(server_str * serv, uint16 port)
{
	struct sockaddr_in addr;
	int i, enable = 1;
	const error *err;

	serv->ls = sys_socket(af_inet, sock_stream | sock_nonblock, 0, &err);
	if (err != nil) {
		fmt_fprint(stderr, "sys_socket failed: %s\n", err->msg);
		return 0;
	}

	/* Disable TIME_WAIT (block port) after incorrect program termination. */
	err = sys_setsockopt(serv->ls, sol_socket, so_reuseaddr, &enable, sizeof(enable));
	if (err != nil) {
		fmt_fprint(stderr, "sys_setsockopt failed: %s\n", err->msg);
		return 0;
	}

	addr.sin_family = af_inet;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = swap_bytes_in_port(port);
	addr.sin_zero[0] = 0L;
	err = sys_bind(serv->ls, (struct sockaddr *) &addr, sizeof(addr));
	if (err != nil) {
		fmt_fprint(stderr, "sys_bind failed: %s\n", err->msg);
		return 0;
	}

	err = sys_listen(serv->ls, 128);
	if (err != nil) {
		fmt_fprint(stderr, "sys_listen failed: %s\n", err->msg);
		return 0;
	}

	for (i = 0; i < max_sess_arr; i++)
		serv->sess_arr[i] = nil;

	return 1;
}

static void server_accept_client(int epfd, server_str * serv, pool * sess_pool)
{
	int conn_sock;
	struct epoll_event ev;
	session *sess;
	const error *err;

	for (;;) {
		conn_sock = sys_accept4(serv->ls, nil, nil, sock_nonblock, &err);
		if (err != nil) {
			if (err->code == EAGAIN)
				break;
			else {
				fmt_fprint(stderr, "sys_accept4 failed: %s\n", err->msg);
				continue;
			}
		}
		if (conn_sock >= max_sess_arr) {
			sys_write(conn_sock, "Connection limit reached, rejecting client\n", 43, nil);
			sys_close(conn_sock);
			continue;
		}

		sess = pool_get(sess_pool);
		if (sess == nil) {
			sys_write(stderr, "Failed to acquire new session\n", 30, nil);
			sys_close(conn_sock);
			continue;
		}
		ev.events = EPOLLIN | EPOLLET;
		ev.data.fd = conn_sock;
		err = sys_epoll_ctl(epfd, epoll_ctl_add, conn_sock, &ev);
		if (err != nil) {
			fmt_fprint(stderr, "sys_epoll_ctl failed: %s\n", err->msg);
			sys_close(conn_sock);
			pool_put(sess_pool, sess);
			continue;
		}
		sess->fd = conn_sock;
		serv->sess_arr[ev.data.fd] = sess;
	}
}

static int server_go(server_str * serv)
{
	int epfd, i;
	struct epoll_event ev, evt[max_events];
	pool *sess_pool;
	session *sess;
	const error *err;

	sess_pool = new_pool((new_pool_item_func) new_session, &err);
	if (err != nil) {
		fmt_fprint(stderr, "new_pool failed: %d\n", err->msg);
		return 2;
	}

	epfd = sys_epoll_create1(0, &err);
	if (err != nil) {
		fmt_fprint(stderr, "sys_epoll_create1 failed: %s\n", err->msg);
		sys_close(serv->ls);
		return 3;
	}

	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = serv->ls;
	err = sys_epoll_ctl(epfd, epoll_ctl_add, serv->ls, &ev);
	if (err != nil) {
		fmt_fprint(stderr, "sys_epoll_ctl failed: %s\n", err->msg);
		sys_close(epfd);
		sys_close(serv->ls);
		return 4;
	}

	for (;;) {
		int ev_count = sys_epoll_wait(epfd, evt, max_events, -1, &err);
		if (err != nil) {
			if (err->code == EINTR)
				continue;
			fmt_fprint(stderr, "sys_epoll_wait failed: %s\n", err->msg);
			sys_close(epfd);
			sys_close(serv->ls);
			return 5;
		}
		for (i = 0; i < ev_count; i++) {
			if (evt[i].data.fd == serv->ls) {
				server_accept_client(epfd, serv, sess_pool);
			} else {
				sess = serv->sess_arr[evt[i].data.fd];

				if (!sesison_do_read(sess)) {
					session_close(sess, serv, sess_pool);
					continue;
				}
				session_write_if_lf(sess);
			}
		}
	}
	return 0;
}

void _start(void)
{
	server_str *serv = new(server_str);

	if (!server_init(serv, 7070))
		sys_exit(1);

	sys_exit(server_go(serv));
}
