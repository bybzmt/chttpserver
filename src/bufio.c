#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include "bufio.h"

static ssize_t buf_read_once(struct bufio *buf);
static ssize_t buf_flush_once(struct bufio *buf);
static void buf_can_read_max(struct bufio *buf, char **bufed, size_t *len);
static void buf_len_del(struct bufio *buf, size_t len);
static void buf_can_write_max(struct bufio *buf, char **bufed, size_t *len);
static void buf_len_add(struct bufio *buf, size_t len);

int buf_init(struct bufio *buf, size_t len, int fd)
{
	buf->cap = len;
	buf->offset = 0;
	buf->len = 0;
	buf->fd = fd;
	buf->buf = (char *)malloc(len);
	if (buf->buf == NULL) {
		return -1;
	}

	return 0;
}

ssize_t buf_readline(struct bufio *buf, const void *data, size_t len)
{
	size_t data_len, data_offset = 0, tmp_len;
	ssize_t write_len;
	char *buf_ptr, *tmp_ptr = NULL;

	data_len = len;

	//无缓冲时读取数据
	if (buf->len == 0) {
READ_AGAIN:
		write_len = buf_read_once(buf);
		if (write_len == 0) {
			errno = errno_unexpect_end;
			return -1;
		} else if (write_len == -1) {
			return -1;
		}
	}

	//将数据复制到缓冲区
	while (data_len > 0 && buf->len > 0) {
		//可用的最大长度
		buf_can_read_max(buf, &buf_ptr, &tmp_len);

		if (tmp_len > data_len) {
			tmp_len = data_len;
		}

		tmp_ptr = (char *)memchr(buf_ptr, '\n', tmp_len);
		if (tmp_ptr != NULL) {
			tmp_len = tmp_ptr - buf_ptr + 1;
		}

		//向缓冲区写数据
		memcpy((char *)data + data_offset, buf_ptr, tmp_len);

		//数据偏移
		data_offset += tmp_len;
		data_len -= tmp_len;

		//缓冲长度减少
		buf_len_del(buf, tmp_len);

		if (tmp_ptr != NULL) {
			break;
		}
	}

	if (tmp_ptr == NULL) {
		if (data_len > 0) {
			goto READ_AGAIN;
		} else {
			errno = errno_data_too_big;
			return -1;
		}
	}

	return data_offset;
}

ssize_t buf_read(struct bufio *buf, const void *data, size_t len)
{
	size_t data_len, data_offset = 0, tmp_len;
	ssize_t write_len;
	char *buf_ptr;

	data_len = len;

	//无缓冲时读取数据
	if (buf->len == 0) {
READ_AGAIN:
		write_len = buf_read_once(buf);
		if (write_len == 0) {
			return data_offset;
		} else if (write_len == -1) {
			return -1;
		}
	}

	//将数据复制到缓冲区
	while (data_len > 0 && buf->len > 0) {
		//可用的最大长度
		buf_can_read_max(buf, &buf_ptr, &tmp_len);

		if (tmp_len > data_len) {
			tmp_len = data_len;
		}

		//向缓冲区写数据
		memcpy((char *)data + data_offset, buf_ptr, tmp_len);

		//数据偏移
		data_offset += tmp_len;
		data_len -= tmp_len;

		//缓冲长度减少
		buf_len_del(buf, tmp_len);
	}

	if (data_len > 0) {
		goto READ_AGAIN;
	}

	return data_offset;
}

ssize_t buf_write(struct bufio *buf, const void *data, size_t len)
{
	size_t data_len, data_offset, tmp_len;
	ssize_t write_len;
	char *buf_ptr;

	data_len = len;
	data_offset = 0;

BUF_AGAIN:
	//将数据复制到缓冲区
	while (data_len > 0 && buf->len < buf->cap) {
		//可用的最大长度
		buf_can_write_max(buf, &buf_ptr, &tmp_len);

		if (tmp_len > data_len) {
			tmp_len = data_len;
		}

		//向缓冲区写数据
		memcpy(buf_ptr, (char *)data + data_offset, tmp_len);

		//数据偏移
		data_offset += tmp_len;
		data_len -= tmp_len;

		//缓冲长度增加
		buf_len_add(buf, tmp_len);
	}

	//如果还有数据未写入缓冲中，则先将一部分实际输出
	if (data_len > 0) {
		write_len = buf_flush_once(buf);
		if (write_len == -1) {
			return -1;
		}

		//再次去写缓冲
		goto BUF_AGAIN;
	}

	return len;
}

//将所有缓冲全布刷出
ssize_t buf_flush(struct bufio *buf)
{
	ssize_t write_len, tmp_len;

	while (buf->len > 0) {
		tmp_len = buf_flush_once(buf);
		if (tmp_len == -1) {
			return -1;
		}

		write_len += tmp_len;
	}

	return write_len;
}

static ssize_t buf_read_once(struct bufio *buf)
{
	size_t tmp_len;
	ssize_t write_len;
	char *buf_ptr;

	//可用的最大长度
	buf_can_write_max(buf, &buf_ptr, &tmp_len);
	if (tmp_len == 0) {
		return 0;
	}

INTR_AGAIN:
	write_len = read(buf->fd, buf_ptr, tmp_len);
	if (write_len == -1) {
		if (errno == EINTR) {
			//中断退出则重来一次
			goto INTR_AGAIN;
		} else {
			return -1;
		}
	}

	buf_len_add(buf, write_len);

	return write_len;
}

//只进行一次刷缓冲行为
static ssize_t buf_flush_once(struct bufio *buf)
{
	size_t tmp_len;
	ssize_t write_len;
	char *buf_ptr;

	buf_can_read_max(buf, &buf_ptr, &tmp_len);

INTR_AGAIN:
	//将数据写出
	write_len = write(buf->fd, buf_ptr, tmp_len);
	if (write_len == -1) {
		if (errno == EINTR) {
			//中断退出则重来一次
			goto INTR_AGAIN;
		} else {
			return -1;
		}
	}

	//缓冲长度减少
	buf_len_del(buf, write_len);

	return write_len;
}

//计算最大可读的连续内存长度
static void buf_can_read_max(struct bufio *buf, char **bufed, size_t *len)
{
	size_t tmp;
	tmp = buf->offset + buf->len;

	//判断是否超出边界
	if (tmp > buf->cap) {
		//当前长度减掉超出边界的部分
		*len = buf->len - (tmp - buf->cap);
	} else {
		*len = buf->len;
	}

	*bufed = buf->buf + buf->offset;
}

static void buf_len_del(struct bufio *buf, size_t len)
{
	//移动启始偏移
	buf->offset += len;
	//减小缓冲数据长度
	buf->len -= len;

	//错误检查:offset超过cap则算法有问题
	assert(!(buf->offset > buf->cap));

	//如果偏移到了缓冲区未尾则归0
	if (buf->cap == buf->offset) {
		buf->offset = 0;
	}
}

//计算最大可写的连续内存
static void buf_can_write_max(struct bufio *buf, char **bufed, size_t *len)
{
	size_t tmp, offset;

	tmp = buf->offset + buf->len;

	//尾端有空闲空间
	if (tmp < buf->cap) {
		offset = buf->offset + buf->len;
		*len = buf->cap - offset;
	//尾端无空闲空间
	} else {
		offset = buf->cap - tmp;
		*len = buf->offset - offset;
	}

	*bufed = buf->buf + offset;
}

static void buf_len_add(struct bufio *buf, size_t len)
{
	//缓冲长度增加
	buf->len += len;
}

