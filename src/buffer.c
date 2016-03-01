#include <unistd.h>
#include <errno.h>
#include <string.h>

//缓冲区可用容量
#define BUFFER_SIZE 4096

struct buffer {
	size_t offset;
	size_t len;
	//总缓存空间容量
	size_t cap;
	//缓存本体
	char *buf;
};

int buffer_init(struct buffer *buf)
{
	buf->offset = 0;
	buf->len = 0;
	buf->cap = BUFFER_SIZE;
	buf->buf = (char *)malloc(BUFFER_SIZE);
	if (buf->buf == NULL) {
		panic(3, "buffer_init malloc fail");
	}

	return 0;
}

//从缓存空间移出数据
size_t buffer_move(struct buffer *buf, void *ptr, size_t len)
{
	size_t mlen;

	//可用数据长度
	if (buf->len < len) {
		mlen = buf->len;
	} else {
		mlen = len;
	}

	if (buf->offset + len > buf->cap) {
		tlen = buf->cap - buf->offset;

		//第1段，从偏移到缓冲未尾
		memcpy(ptr, buf->buf[buf->offset], tlen);

		//第2段，从缓冲开头开始
		memcpy(ptr + tlen, buf->buf[0], mlen - tlen);
	} else {
		memcpy(ptr, buf->buf[buf->offset], mlen);
	}

	buf->offset += mlen;
	if (buf->offset >= buf->cap) {
		buf->offset -= buf->cap;
	}

	buf->len -= mlen;

	return mlen;
}

//从文件读入数据到缓存
ssize_t buffer_read(struct buffer *buf, int fd)
{
	size_t offset, len;

	//空闲空间长度
	len = buf->cap - buf->len;
	if (len == 0) {
		return 0;
	}

	//空闲空间位置
	offset = buf->offset + buf->len;
	if (offset > buf->cap) {
		offset -= buf->cap;
	}

	//判断是分块
	if (offset + len > buf->cap) {
		return _buffer_readv(buf, fd, offset, len);
	} else {
		return _buffer_read(buf, fd, offset, len);
	}
}

inline ssize_t _buffer_read(struct buffer *buf, int fd, size_t offset, size_t len)
{
	ssize_t size;

INTR_AGAIN:
	size = read(fd, buf->buf[offset], len);
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
inline ssize_t _buffer_readv(struct buffer *buf, int fd, size_t offset, size_t len)
{
	ssize_t size;
	struct iovec tmp[2];

	tmp[0].iov_base = buf->buf[offset];
	tmp[0].iov_len = buf->cap - offset;

	tmp[1].iov_base = buf->buf[0];
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

//查找字符串，反回从offset开的长度
ssize_t buffer_chr(struct buffer *buf, char chr)
{
	char *ptr;

	if (buf->offset + buf->len > buf->cap) {
		tlen = buf->cap - buf->offset;

		//第1段，从偏移到缓冲未尾
		ptr = (char *)memchr(buf->buf[buf->offset], chr, tlen);
		if (ptr != NULL) {
			return ptr - buf->[buf->offset];
		}

		//第2段，从缓冲开头开始
		ptr = (char *)memchr(buf->buf[0], chr, buf->len - tlen);
		if (ptr != NULL) {
			return tlen + (ptr - buf->buf[0]);
		}
	} else {
		ptr = (char *)memchr(buf->buf[buf->offset], chr, buf->len);
		if (ptr != NULL) {
			return ptr - buf->[buf->offset];
		}
	}

	return -1;
}

ssize_t buffer_strpos(struct buffer *buf, char *se, size_t se_len)
{
	ssize_t offset;

	if (buf->offset + buf->len > buf->cap) {
		tlen = buf->cap - buf->offset;

		//第1段，从偏移到缓冲未尾
		offset = _buffer_strpos(buf->buf[buf->offset], tlen, se, se_len);
		if (offset != -1) {
			return offset;
		}

		//第2段，从缓冲开头开始
		offset = _buffer_strpos(buf->buf[0], buf->len - tlen, se, se_len);
		if (offset != -1) {
			return tlen + offset;
		}
	} else {
		offset = _buffer_strpos(buf->buf[buf->offset], buf->len, se, se_len);
		if (offset != -1) {
			return offset;
		}
	}

	return -1;
}

ssize_t _buffer_strpos(char *str, size_t len, char *se, size_t se_len)
{
	for (size_t i=0; i < len; i++) {
		for (size_t x=0; x < se_len; x++) {
			if (*str == se[x]) {
				return i;
			}
		}

		str++;
	}

	return -1;
}
