#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include "base.h" 

typedef struct _mapdata {
	uint32_t h;
	//在当前槽中的下一个
	MapData *next;
	MapData *prev;
	//在整个map中的上一个
	MapData *listNext;
	MapData *listPrev;
	void *data;
	uint16_t klen;
	char key[1];
} MapData;

typedef struct _hash_map {
	size_t len;
	size_t cap;
	MapData *next;
	MapData *data[];
} Map;

void map_init(Map *map);

void map_add(Map *map, char key[], size_t len, void *value);

int map_add_unique(Map *map, char key[], size_t len, void *value);

MapData *map_get(Map *map, char key[], size_t len);

int map_get_data(Map *map, char key[], size_t len, void *data);

int map_del(Map *map, char key[], size_t len);

void map_close(Map *map);


#endif
