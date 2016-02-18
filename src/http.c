#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <setjmp.h>

#include "http.h"
#include "bufio.h"

static ssize_t writen(int fd, const void *data, size_t len);

struct TCPConn {
	int fd;
	struct bufio rbuf;
	struct bufio wbuf;
	struct client *client;
};


struct _request {
	//请求方法
	Data Method;
	//请求URI
	Data RequestURI;
	//协议
	Data Proto;
	//请求头
	Map Header;
	//请求体
	Body Body;
	//请求尾
	Map Trailer;
	//请求状态
	char _status;
	_Bool Close;
} Request;

struct HTTPConn {
	struct TCPConn conn;
	struct Request req;
	struct Response resp;
};

void http_serve(Ctx *ctx)
{
	ssize_t n;
	struct TCPConn conn;
	Exception e;

	conn.client = client;
	buf_init(&conn.rbuf, 4096, client->connfh);
	buf_init(&conn.wbuf, 4096, client->connfh);

	exception_init(&e);
	re = sigsetjmp(e.jmpbuf, 0);
	if (re == 0) {
		http_parse(ctx);
	} else {
		exception_recover(&e);
	}

	free(conn.rbuf.buf);
	free(conn.wbuf.buf);
	close(conn.client->connfh);
}

void http_parse(struct TCPConn *conn)
{
	size_t len=0, cap=0;

	http_parse_request(req);
	http_parse_headers(req);

	http_run(req, resp);
}

void http_parse_request(struct Request *req)
{
	tmp = buffer_readString(req->rbuf, req->fd, ' ', req->method);
	if (tmp == -1) {
		fprintf(stderr, "request method error.");
		return;
	}

	tmp = buffer_readString(req->rbuf, req->fd, ' ', req->RequestURI);
	if (tmp == -1) {
		fprintf(stderr, "request uri error.");
		return;
	}

	tmp = buffer_readStringLine(req->rbuf, req->fd, req->Proto);
	if (tmp == -1) {
		fprintf(stderr, "request protol error.");
		return;
	}
}

void http_parse_headers(struct Request *req)
{
}

static ssize_t writen(int fd, const void *data, size_t len)
{
	size_t need_write;
	const char *ptr;
	ssize_t n;

	ptr = data;
	need_write = len;

	while (need_write > 0) {
		n = write(fd, ptr, need_write);

		if (n > 0) {
			need_write -= n;
			ptr += n;
		} else {
			if (n < 0 && errno == EINTR) {
				n = 0;
			} else {
				return -1;
			}
		}
	}

	return len;
}
