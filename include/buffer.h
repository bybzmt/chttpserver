#ifndef BUFFER_H
#define BUFFER_H

//缓冲区可用容量
#define BUFFER_SIZE 4096

struct buffer {
	//内容开始
	void *ptr;
	//内容长度
	size_t len;
	//总缓存空间容量
	size_t cap;
	//缓冲开始
	void *start;
	//缓冲结束
	void *stop;
};

int buffer_init(struct buffer *buf);

ssize_t buffer_push_from_read(struct buffer *buf, int fd);

ssize_t buffer_chrpos(struct buffer *buf, char ch, size_t offset);

void *buffer_chr(struct buffer *buf, size_t offset, char ch);

void *buffer_chrs(struct buffer *buf, size_t offset, char *chs);

size_t buffer_shift_len(struct buffer *buf, void *ptr, size_t len);

size_t buffer_shift(struct buffer *buf, void *ptr, void *stop);

size_t buffer_shift_trash(struct buffer *buf, size_t len);

void buffer_destroy(struct buffer *buf);

#endif
