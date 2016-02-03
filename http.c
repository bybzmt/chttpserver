#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "http.h"

struct buf {
	int len;
	int start;
	int end;
	char *buf;
};

//从缓冲区读出一行数据，函数以"\r\n"作为行分隔
//成功返回读取到的字节数
//发生错误反回-1
static ssize_t buf_readline(struct buf *buf, const void *data, size_t len);

//从缓冲区读出数据
//成功返回读取到的字节数
//发生错误反回-1
static ssize_t buf_read(struct buf *buf, const void *data, size_t len);

//向缓冲区写入数据，函数总是将数据全布写入缓冲才返回
//返回写入的字节数
//发生错误反回-1
static ssize_t buf_write(struct buf *buf, const void *data, size_t len);

//将所有缓冲写入
//发生错误反回-1
static ssize_t buf_flush(struct buf *buf);

static ssize_t writen(int fd, const void *data, size_t len);

void http_serve(struct client * client)
{
	ssize_t n;
	char buf[] = "Hello Wrold!\n";

	n = writen(client->connfd, buf, strlen(buf));
	if (n == -1) {
		printf("call writen errno:%d\n", errno);
		return;
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
