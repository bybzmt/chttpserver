#ifndef CHUNK_BUFFER_H
#define CHUNK_BUFFER_H

#include <stdint.h>
#include <stddef.h>
#include "chunk_data.h"

//缓存大小
#define BUFFER_MIN 2048
#define BUFFER_CHUNK_SIZE 4096
#define BUFFER_CHUNK_DEFAULT_SLOT 16

//分块缓存
struct chunk_buffer {
	size_t len;
	//块大小
	uint16_t chunk_size;
	//块数量
	uint16_t chunk_num;
	//块槽数量
	uint16_t chunk_slot;
	//缓冲区指针的指针
	char **bufs;
};

//分块缓存初始化
void chunk_buffer_init(struct chunk_buffer *buf);

//在缓存数据中查找字符
ssize_t chunk_buffer_pos(const struct chunk_buffer *buf, size_t offset, int search);

//读取更多数据到缓存中
ssize_t chunk_buffer_readMore(struct chunk_buffer *buf, int fd);

//消毁数据
void chunk_buffer_destroy(struct chunk_buffer *buf);

//标记指定位置的缓存数据为分块数据
struct chunk_data *chunk_buffer_to_chunk_data(const struct chunk_buffer *buf, size_t offset, size_t len);

//复制指定拉置的数据
chunk_buffer_to_cpy(const struct chunk_buffer *buf, void *dst, size_t offset, size_t len);

//到指定位置的字符
char chunk_buffer_at(const struct chunk_buffer *buf, size_t offset);

#endif
