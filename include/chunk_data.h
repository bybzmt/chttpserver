#ifndef CHUNK_DATA_H
#define CHUNK_DATA_H

#include <stdint.h>
#include <stddef.h>

//指定长度数据
struct len_data {
	uint16_t len;
	char *data;
};

//分块数据
struct chunk_data {
	//数据总长度
	size_t len;
	//分块数量
	uint16_t chunk_num;
	//分块指针
	struct len_data chunks[1];
};

//创建分块数据 参数:num 是块数量
struct chunk_data *chunk_data_create(uint16_t num);

#endif
