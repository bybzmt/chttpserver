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
	struct chunk_data Method;
	//请求URI
	struct chunk_data RequestURI;
	//协议
	struct chunk_data Proto;
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
		//出问题了
	}

	exception_destroy(&e);

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
	ssize_t offset, pos;
	size_t len;

	offset = 0;

	pos = read_unti_char(buf, req->fd, offset, ' ', 10);

	req->method = chunk_buffer_to_chunk_data(buf, offset, pos - offset);

	//跳过刚的找到的空格
	offset = pos + 1;
	
	pos = read_unti_char(buf, req->fd, offset, ' ', 1024*8);

	req->requestURI = chunk_buffer_to_chunk_data(buf, offset, pos - offset);

	//跳过刚的找到的空格
	offset = pos + 1;
	
	pos = read_unti_char(buf, req->fd, offset, '\n', 1024*8);

	req->Proto = chunk_buffer_to_chunk_data(buf, offset, pos - offset);

	//修正未尾换行
	fix_line_end(&req->Proto);

	offset = pos + 1;
}

//读取直到某个字符停下来
size_t read_unti_char(struct chunk_buffer *buf, int fd, size_t offset, char code, size_t max_size)
{
	ssize_t pos;

	for (;;) {
		pos = chunk_buffer_pos(buf, offset, code);
		if (pos == -1) {
			if (buf->len > max_size) {
				painc(2, "request too large.");
			} else {
				//查找指针指向内容未尾
				offset = buf->len;

				pos = chunk_buffer_readMore(buf, req->fd);
				if (pos < 1) {
					painc(2, "request io error.");
				}
			}
		} else {
			return pos;
		}
	}
}

//修正未尾的换行符问题
void fix_line_end(struct chunk_data *chunk)
{
	struct len_data *tmp;
	tmp = &chunk->chunks[chunk->chunk_num-1];
	if (tmp->data[tmp.len-1] == '\r') {
		tmp->len--;
		chunk->len--;
	}
}
void fix_line_end(struct chunk_buffer *buf)
{
	struct len_data *tmp;
	tmp = &chunk->chunks[chunk->chunk_num-1];
	if (tmp->data[tmp.len-1] == '\r') {
		tmp->len--;
		chunk->len--;
	}
}

void http_parse_headers(struct chunk_buffer *buf, int fd, size_t offset, struct Request *req)
{
	map_init(&req->Header);

	for (;;) {
		pos = read_unti_char(buf, req->fd, offset, '\n', 1024*1024);

		if (chunk_buffer_at(buf, pos - 1) == "\r") {
			r_pos = pos - 1;
		}

		len = r_pos - offset;

		//空行表示头结束
		if (len == 0) {
			break;
		}

		if (len > 255) {
			panic(3, "header name too long");
		}

		key = (char *) malloc(len);
		if (key == NULL) {
			panic(3, "parse_header malloc");
		}

		chunk_buffer_to_data(buf, key, offset, len);

		map_add(&req->Header, key, len);

		free(key);
	}
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
