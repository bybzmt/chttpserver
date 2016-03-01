#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/uio.h>
#include "exception.h" 
#include "chunk_buffer.h" 
#include "chunk_data.h" 


//读取数据（非聚合读）
static ssize_t _chunk_buffer_read(struct chunk_buffer *buf, int fd);

//读取数据（聚合读）
static ssize_t _chunk_buffer_readv(struct chunk_buffer *buf, int fd);

//增加1块缓冲内存
static void _chunk_buffer_resize(struct chunk_buffer *buf);

//分块缓存初始化
void chunk_buffer_init(struct chunk_buffer *buf)
{
	buf->len = 0;
	buf->chunk_num = 0;
	buf->chunk_size = BUFFER_CHUNK_SIZE;
	buf->chunk_slot = BUFFER_CHUNK_DEFAULT_SLOT;

	buf->bufs = (char **)malloc(sizeof(char *) * buf->chunk_slot);
	if (buf->bufs == NULL) {
		panic(1, "chunk_buffer_init malloc error");
	}
}

//复制指定位置的缓存数据到连续的内存
struct chunk_data *chunk_buffer_to_data(const struct chunk_buffer *buf, size_t offset, size_t len)
{
	uint16_t chunk_start_i, chunk_start_offset, chunk_start_len, chunk_end_i, chunk_end_len;
	struct chunk_data *data;
	int num;

	if (offset > buf->len) {
		panic(3, "chunk_buffer_to_chunk_data offset > len");
	}
	if (offset + len > buf->len) {
		panic(3, "chunk_buffer_to_chunk_data offset+len > len");
	}

	//找到起始页和起始页偏移
	chunk_start_i = offset / buf->chunk_size;
	chunk_start_offset = offset % buf->chunk_size;
	chunk_start_len = buf->chunk_size - chunk_start_offset;

	//找到结束页和结束页长度
	chunk_end_i = (offset + len - 1) / buf->chunk_size;
	chunk_end_len = (offset + len) % buf->chunk_size;
	if (chunk_end_len == 0) {
		chunk_end_len = buf->chunk_size;
	}

	//涉及到的数据块数量(-1)
	num = chunk_end_i - chunk_start_i;

	data = (char *)malloc(len);
	if (data == NULL) {
		panic(3, "chunk_buffer_to_data malloc fail");
	}

	//起始块
	memcpy(data, buf->bufs[chunk_start_i] + chunk_start_offset, chunk_start_len);

	data_ptr = data + chunk_start_len;

	//其它中间块儿
	for (int i=1; i < num; i++) {
		memcpy(data_ptr, buf->bufs[chunk_start_i + i], buf->chunk_size);

		data_ptr = data + buf->chunk_size;
	}

	//结束块
	if (num > 0) {
		memcpy(data_ptr, buf->bufs[chunk_end_i], chunk_end_len);
	}

	return data;
}

//标记指定位置的缓存数据为分块数据
struct chunk_data *chunk_buffer_to_chunk_data(const struct chunk_buffer *buf, size_t offset, size_t len)
{
	uint16_t chunk_start_i, chunk_start_offset, chunk_start_len, chunk_end_i, chunk_end_len;
	struct chunk_data *data;
	int num;

	if (offset > buf->len) {
		panic(3, "chunk_buffer_to_chunk_data offset > len");
	}
	if (offset + len > buf->len) {
		panic(3, "chunk_buffer_to_chunk_data offset+len > len");
	}

	//找到起始页和起始页偏移
	chunk_start_i = offset / buf->chunk_size;
	chunk_start_offset = offset % buf->chunk_size;
	chunk_start_len = buf->chunk_size - chunk_start_offset;

	//找到结束页和结束页长度
	chunk_end_i = (offset + len - 1) / buf->chunk_size;
	chunk_end_len = (offset + len) % buf->chunk_size;
	if (chunk_end_len == 0) {
		chunk_end_len = buf->chunk_size;
	}

	//涉及到的数据块数量(-1)
	num = chunk_end_i - chunk_start_i;

	data = chunk_data_create(num + 1);
	data->len = len;

	//起始块
	data->chunks[0].data = buf->bufs[chunk_start_i] + chunk_start_offset;
	data->chunks[0].len = chunk_start_len;

	//其它中间块儿
	for (int i=1; i < num; i++) {
		data->chunks[i].data = buf->bufs[chunk_start_i + i];
		data->chunks[i].len = buf->chunk_size;
	}

	//结束块
	if (num > 0) {
		data->chunks[num].data = buf->bufs[chunk_end_i];
		data->chunks[num].len = chunk_end_len;
	}

	return data;
}

//在缓存数据中查找字符
ssize_t chunk_buffer_pos(const struct chunk_buffer *buf, size_t offset, int search)
{
	uint16_t chunk_start_i, chunk_start_offset, chunk_start_len, chunk_end_i, chunk_end_len;
	char *tmp_ptr;
	int num;

	if (offset > buf->len) {
		return -1;
	}

	//找到起始页和起始页偏移
	chunk_start_i = offset / buf->chunk_size;
	chunk_start_offset = offset % buf->chunk_size;
	chunk_start_len = buf->chunk_size - chunk_start_offset;

	//找到结束页和结束页长度
	chunk_end_i = buf->chunk_num;
	chunk_end_len = buf->len % buf->chunk_size;
	if (chunk_end_len == 0) {
		chunk_end_len = buf->chunk_size;
	}

	//涉及到的数据块数量(-1)
	num = chunk_end_i - chunk_start_i;

	//起始页
	tmp_ptr = (char *)memchr(buf->bufs[chunk_start_i] + chunk_start_offset, search, chunk_start_len);
	if (tmp_ptr != NULL) {
		return (ssize_t)buf->chunk_size * chunk_start_i + (tmp_ptr - buf->bufs[chunk_start_i]);
	}

	//中间页
	for (int i=1; i < num; i++) {
		tmp_ptr = (char *)memchr(buf->bufs[chunk_start_i + i], search, buf->chunk_size);
		if (tmp_ptr != NULL) {
			return (ssize_t)buf->chunk_size * (chunk_start_i + i) + (tmp_ptr - buf->bufs[chunk_start_i + i]);
		}
	}

	//结束页
	if (num > 0) {
		tmp_ptr = (char *)memchr(buf->bufs[chunk_end_i], search, chunk_end_len);
		if (tmp_ptr != NULL) {
			return (ssize_t)buf->chunk_size * chunk_end_i + (tmp_ptr - buf->bufs[chunk_end_i]);
		}
	}

	return -1;
}

//读取更多数据到缓存中
inline ssize_t chunk_buffer_readMore(struct chunk_buffer *buf, int fd)
{
	if (buf->chunk_num * buf->chunk_size - buf->len < BUFFER_MIN) {
		_chunk_buffer_resize(buf);

		return _chunk_buffer_readv(buf, fd);
	} else {
		return _chunk_buffer_read(buf, fd);
	}
}

//消毁数据
void chunk_buffer_destroy(struct chunk_buffer *buf)
{
	for (int i=0; i < buf->chunk_num; i++) {
		free(buf->bufs[i]);
	}

	free(buf->bufs);
}

//读取数据（非聚合读）
ssize_t _chunk_buffer_read(struct chunk_buffer *buf, int fd)
{
	uint16_t chunk_i, chunk_offset, cap;
	ssize_t size;
	void *ptr;

	chunk_i = buf->len / buf->chunk_size;
	chunk_offset = buf->len % buf->chunk_size;

	ptr = buf->bufs[chunk_i] + chunk_offset;
	cap = buf->chunk_size - chunk_offset;

	//读取数据
INTR_AGAIN:
	size = read(fd, ptr, cap);
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

//读取数据（聚合读）
ssize_t _chunk_buffer_readv(struct chunk_buffer *buf, int fd)
{
	uint16_t chunk_i, chunk_offset;
	ssize_t size;
	struct iovec tmp[2];

	chunk_i = buf->len / buf->chunk_size;
	chunk_offset = buf->len % buf->chunk_size;

	tmp[0].iov_base = buf->bufs[chunk_i] + chunk_offset;
	tmp[0].iov_len = buf->chunk_size - chunk_offset;

	tmp[1].iov_base = buf->bufs[chunk_i+1];
	tmp[1].iov_len = buf->chunk_size;

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

//增加1块缓冲内存
inline void _chunk_buffer_resize(struct chunk_buffer *buf)
{
	//查看槽数量是否足够
	if (buf->chunk_num >= buf->chunk_slot) {
		buf->chunk_slot *= 2;

		buf->bufs = (char **)realloc(buf->bufs, sizeof(char *) * buf->chunk_slot);
		if (buf->bufs == NULL) {
			panic(1, "_chunk_buffer_resize malloc error");
		}
	}

	//增加内存块
	buf->bufs[buf->chunk_num] = (char *)malloc(sizeof(char) * buf->chunk_size);
	if (buf->bufs[buf->chunk_num] == NULL) {
		panic(1, "_chunk_buffer_resize malloc error");
	}
	buf->chunk_num++;
}


