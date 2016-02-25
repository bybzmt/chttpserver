#include <stdlib.h>
#include <stdint.h>
#include "exception.h" 
#include "chunk_data.h" 

//创建分块数据 参数:num 是块数量
inline struct chunk_data *chunk_data_create(uint16_t num)
{
	struct chunk_data * data;

	data = (struct chunk_data *) malloc(sizeof(struct chunk_data) + (sizeof(struct len_data) * (num-1)));
	if (data == NULL) {
		panic(1, "chunk_data_create malloc error");
	}

	data->chunk_num = num;

	return data;
}

/*
void chunk_data_cpy()
{
}

void chunk_data_cmp()
{
}

void chunk_data_set()
{
}

void chunk_data_pos()
{
}
*/
