#ifndef MY_HASH_H
#define MY_HASH_H

#include <stdint.h>
#include <stdlib.h>

//fnv1a hash算法
uint32_t fnv1a_hash(char data[], size_t len);

#endif
