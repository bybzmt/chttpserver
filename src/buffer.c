#include <unistd.h>
#include <errno.h>
#include <string.h>

//初始缓存8k
#define BUFFER_DEFAULT (1<<13) 
//缓存增长大小
#define BUFFER_APPEND 4096
//最小缓冲区可用容量
#define BUFFER_MIN 2048
//缓存最大1M
#define BUFFER_MAX (1<<20)

//缓存越过最大值
#define E_BUFFER_MAX 10001;
//读到结尾
#define E_BUFFER_EOF = 10002;

struct buffer {
	//外部读取偏移
	size_t offset;
	//己存内容长度
	size_t len;
	//缓存总容量
	size_t cap;
	//缓存本体
	char buf[];
};

struct chunk {
	size_t offset;
	size_t len;
};

struct String {
	size_t len;
	char data[1];
};

int buf_init(struct buffer *buf)
{
	buf->offset = 0;
	buf->len = 0;
	buf->cap = BUFFER_DEFAULT;
	buf->buf = (char *)malloc(BUFFER_DEFAULT);
	if (buf->buf == NULL) {
		return -1;
	}

	return 0;
}

int buf_append(struct buffer *buf)
{
	buf->cap += BUFFER_APPEND;
	if (buf->cap > BUFFER_MAX) {
		errno = E_BUFFER_MAX;
		return -1;
	}

	buf->buf = (char *)realloc(buf->buf, buf->cap);
	if (buf->buf == NULL) {
		return -1;
	}

	return 0;
}

int buffer_readString(struct bufio *buf, int fd, int find, struct StringPtr *str)
{
	size_t data_offset = 0;
	ssize_t tmp;
	char *tmp_ptr;
	struct StringPtr str;

	for (;;) {
		//查找字符
		if (buf->len > buf->offset) {
			tmp_ptr = (char *)memchr(buf->buf[buf->offset], find, buf->len - buf->offset);
			if (tmp_ptr != NULL) {
				str->data = buf->buf[buf->offset];
				str->len = tmp_ptr - buf->buf[buf->offset];

				buf->offset += str->len + 1;

				return 0;
			}
		}

		//检查缓存空闲容量是否足够
		if (buf->len+BUFFER_MIN > buf->cap) {
			tmp = buf_append(buf);
			if (tmp == -1) {
				return -1;
			}
		}

		//读取数据
INTR_AGAIN:
		tmp = read(fd, buf->buf[buf->len], buf->cap - buf->len);
		if (tmp == 0) {
			errno = E_BUFFER_EOF;
			return -1;
		} else if (tmp == -1) {
			if (errno == EINTR) {
				//中断退出则重来一次
				goto INTR_AGAIN;
			} else {
				return -1;
			}
		}

		buf->len + tmp;
	}
}

int buffer_readStringLine(struct bufio *buf, int fd, struct StringPtr *str)
{
	buffer_readString(buf, fd, '\n', str);

	//修正\r\n问题
	if (str->data[str->len-1] == '\r') {
		str->len--;
	}
}

ssize_t buffer_read(struct bufio *buf, const void *data, size_t len)
{
	ssize_t tmp;

	//检查缓存空闲容量是否足够
	if (buf->len+BUFFER_APPEND > buf->cap) {
		tmp = buf_append(buf);
		if (tmp == -1) {
			return -1;
		}
	}

	tmp = read(fd, buf->buf[buf->len], buf->cap - len);
	if (tmp == 0) {
		errno = E_BUFFER_EOF;
		return 0;
	} else if (tmp == -1) {
		return -1;
	}

	return tmp;
}

