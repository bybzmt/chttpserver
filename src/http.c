#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "http.h"

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
