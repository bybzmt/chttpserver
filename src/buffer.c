#include <unistd.h>
#include <errno.h>
#include "mystring.h"
#include "buffer.h"

static ssize_t _buffer_read(struct buffer *buf, int fd, void *ptr, size_t len);
static ssize_t _buffer_readv(struct buffer *buf, int fd, void *ptr, size_t len);

int buffer_init(struct buffer *buf)
{
	assert(buf != NULL);

	buf->len = 0;
	buf->cap = BUFFER_SIZE;
	buf->start = malloc(BUFFER_SIZE);
	if (buf->buf == NULL) {
		panic(3, "buffer_init malloc fail");
	}

	buf->stop = buf->start + buf->cap;
	buf->ptr = buf->start;

	return 0;
}

//读入数据到缓存尾部
ssize_t buffer_push_from_read(struct buffer *buf, int fd)
{
	void *ptr;
	size_t len;

	//空闲空间长度
	len = buf->cap - buf->len;
	if (len < 1) {
		return 0;
	}

	//空闲空间位置
	ptr = buf->ptr + buf->len;
	if (ptr >= buf->stop) {
		ptr -= buf->cap;
	}

	//判断是分块
	if (ptr + len > buf->stop) {
		return _buffer_readv(buf, fd, ptr, len);
	} else {
		return _buffer_read(buf, fd, ptr, len);
	}
}

inline ssize_t _buffer_read(struct buffer *buf, int fd, void *ptr, size_t len)
{
	ssize_t size;

INTR_AGAIN:
	size = read(fd, ptr, len);
	if (size == -1) {
		if (errno == EINTR) {
			//中断退出则重来一次
			goto INTR_AGAIN;
		} else {
			return -1;
		}
	}

	buf->len += size;

	return size;
}

//聚合读
inline ssize_t _buffer_readv(struct buffer *buf, int fd, void *ptr, size_t len)
{
	ssize_t size;
	struct iovec tmp[2];

	tmp[0].iov_base = ptr;
	tmp[0].iov_len = buf->stop - ptr;

	tmp[1].iov_base = buf->start;
	tmp[1].iov_len = len - tmp[0].iov_len;

	//读取数据
INTR_AGAIN:
	size = readv(fd, (struct iovec *)&tmp, 2);
	if (size == -1) {
		if (errno == EINTR) {
			//中断退出则重来一次
			goto INTR_AGAIN;
		} else {
			return -1;
		}
	}

	buf->len += size;

	return size;
}

//查找字符，反回从要查的字符的位置
ssize_t buffer_chrpos(struct buffer *buf, char ch, size_t offset)
{
	void *ptr, *tmp;
	size_t len;

	if (offset >= buf->len) {
		return -1;
	}

	//可用数据长度
	len = buf->len - offset;

	//要查找的偏移位置
	ptr = buf->ptr + offset;
	if (ptr >= buf->stop) {
		ptr -= buf->cap;
	}

	//判断是否越界
	if (ptr + len > buf->stop) {
		tlen = buf->stop - ptr;

		//第1段，从偏移到缓冲未尾
		tmp = memchr(ptr, ch, tlen);
		if (tmp != NULL) {
			return offset + (tmp - ptr);
		}

		//第2段，从缓冲开头开始
		tmp = memchr(buf->start, ch, len - tlen);
		if (tmp != NULL) {
			return offset + tlen + (tmp - buf->start);
		}
	} else {
		tmp = memchr(ptr, ch, len);
		if (tmp != NULL) {
			return tmp - ptr;
		}
	}

	return -1;
}

//查找字符，反回找到的指针
void *buffer_chr(struct buffer *buf, size_t offset, char ch)
{
	void *start, *tmp;
	size_t len;

	if (buf->len < 1) {
		return NULL;
	}

	//开始位置
	start = buf->ptr + offset;
	if (start >= buf->stop) {
		start = buf->ptr;
	}

	//结束位置
	stop = buf->ptr + buf->len;
	if (stop > buf->stop) {
		stop -= buf->cap;
	}

	//判断是否越界
	if (start < stop) {
		tmp = memchr(start, ch, stop - start);
		if (tmp != NULL) {
			return tmp;
		}
	} else {
		//第1段，从偏移到缓冲未尾
		tmp = memchr(start, ch, buf->stop - start);
		if (tmp != NULL) {
			return tmp;
		}

		//第2段，从缓冲开头开始到结束
		tmp = memchr(buf->start, ch, stop - buf->start);
		if (tmp != NULL) {
			return start;
		}
	}

	return NULL;
}

//查找字符，反回找到的指针
void *buffer_chrs(struct buffer *buf, size_t offset, char *chs)
{
	void *start, *tmp;
	size_t len;

	if (buf->len < 1) {
		return NULL;
	}

	//开始位置
	start = buf->ptr + offset;
	if (start >= buf->stop) {
		start = buf->ptr;
	}

	//结束位置
	stop = buf->ptr + buf->len;
	if (stop > buf->stop) {
		stop -= buf->cap;
	}

	//判断是否越界
	if (start < stop) {
		tmp = memchrs(start, chs, stop - start);
		if (tmp != NULL) {
			return tmp;
		}
	} else {
		//第1段，从偏移到缓冲未尾
		tmp = memchrs(start, chs, buf->stop - start);
		if (tmp != NULL) {
			return tmp;
		}

		//第2段，从缓冲开头开始到结束
		tmp = memchrs(buf->start, chs, stop - buf->start);
		if (tmp != NULL) {
			return start;
		}
	}

	return NULL;
}

//从缓冲头部移出数据
//从buf->ptr到stop为止
size_t buffer_shift(struct buffer *buf, void *ptr, void *stop, size_t ptr_len)
{
	size_t len, tlen;

	assert(buf != NULL);

	if (buf->ptr == stop) {
		return 0;
	}

	if (buf->ptr < stop) {
		len = stop - buf->ptr;

		memcpy(ptr, buf->start, len);
	} else {
		len = buf->stop - buf->ptr;

		//第1段，从偏移到缓冲未尾
		memcpy(ptr, buf->ptr, len);

		tlen = stop - buf->start;

		//第2段，从缓冲开头开始
		memcpy(ptr + len, buf->start, tlen);

		len += tlen;
	}

	buf->len -= len;

	buf->ptr += len;
	if (buf->ptr >= buf->stop) {
		buf->ptr -= buf->cap;
	}

	return len;
}

//从缓冲头部移出数据
size_t buffer_shift_len(struct buffer *buf, void *ptr, size_t len)
{
	size_t tlen;

	assert(buf != NULL);

	//可用数据长度
	if (buf->len < len) {
		len = buf->len;
	}

	if (buf->ptr + len > buf->stop) {
		tlen = buf->stop - buf->ptr;

		//第1段，从偏移到缓冲未尾
		memcpy(ptr, buf->ptr, tlen);

		//第2段，从缓冲开头开始
		memcpy(ptr + tlen, buf->start, len - tlen);
	} else {
		memcpy(ptr, buf->start, len);
	}

	buf->len -= len;

	buf->ptr += len;
	if (buf->ptr >= buf->stop) {
		buf->ptr -= buf->cap;
	}

	return len;
}

//从缓冲头部移出数据
//从buf->ptr到stop为止
size_t buffer_shift_trash(struct buffer *buf, size_t len)
{
	assert(buf != NULL);

	//可用数据长度
	if (buf->len < len) {
		len = buf->len;
	}

	buf->len -= len;

	buf->ptr += len;
	if (buf->ptr >= buf->stop) {
		buf->ptr -= buf->cap;
	}

	return len;
}

//消毁数据
inline void buffer_destroy(struct buffer *buf)
{
	free(buf->start);
}
