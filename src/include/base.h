#ifndef HASH_H
#define HASH_H

#include <stdint.h>

typedef struct _data {
	uint32_t len;
	char   data[1];
} Data;

void base_init();

#endif
