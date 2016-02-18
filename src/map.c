#include <stdint.h>
#include "base.h" 
#include "hash.h" 
#include "exception.h" 

static void _map_resize(Map *map);

static MapData *_do_mapdata_init(Map *map, char key[], size_t len, void *value);
static void _do_mapdata_add(Map *map, MapData *d);
static MapData *_do_mapdata_get(Map *map, uint32_t h, char key[], size_t len);
static void _do_mapdata_del(Map *map, MapData *d);

void map_init(Map *map)
{
	map->len = 0;
	map->cap = 8;
	map->data = (MapData **) malloc(sizeof(MapData) * map->cap);
	if (map->data == NULL) {
		panic(1, "map_init malloc error");
	}
}

void map_add(Map *map, char key[], size_t len, void *value)
{
	MapData *d;
	uint32_t h;

	if (map->len * 0.75 > map->cap) {
		_map_resize(map);
	}

	h = fnv1a_hash(key, len);

	d = _do_mapdata_init(map, h, key, len, value);

	_do_mapdata_add(map, d);
}

int map_add_unique(Map *map, char key[], size_t len, void *value)
{
	MapData *d;
	int re;
	uint32_t h;

	if (map->len * 0.75 > map->cap) {
		_map_resize(map);
	}

	h = fnv1a_hash(key, len);

	d = _do_mapdata_get(map, h, key, len);
	if (d != NULL) {
		d->data = value;
		return 1;
	}

	d = _do_mapdata_init(map, h, key, len, value);
	_do_mapdata_add(map, d);

	return 0;
}

MapData *map_get(Map *map, char key[], size_t len)
{
	int i;

	i = fnv1a_hash(key, len) % map->cap;

	return _do_mapdata_get(map, i, key, len);
}

int map_get_data(Map *map, char key[], size_t len, void *data)
{
	MapData *d;
	int i;

	i = fnv1a_hash(key, len) % map->cap;

	d = _do_mapdata_get(map, i, key, len);

	if (d != NULL) {
		data = d->data
		return 1;
	}

	return 0;
}

int map_del(Map *map, char key[], size_t len)
{
	int i;
	i = fnv1a_hash(key, len) % map->cap;

	d = map->data[i];

	while (d != NULL) {
		if ((len == d->klen) && (memcmp(d->key, key, len) == 0)) {
			_do_mapdata_del(d);

			return 1;
		}

		d = d->next;
	}

	return 0;
}

void map_close(Map *map)
{
	MapData *d, *e;

	d = map->next;

	while (d != NULL) {
		e = d->prev;

		free(d);

		d = e;
	}

	free(map->data);
}

static void _map_resize(Map *map)
{
	MapData *d;

	map->cap *= 2;

	free(map->data);

	map->data = (MapData **)calloc(map->cap, sizeof(MapData *));
	if (map->data == NULL) {
		panic(1, "_map_resize malloc error");
	}

	d = map->next;

	while (d != NULL) {
		_do_mapdata_add(map, d);

		d = d->next;
	}
}

static inline MapData *_do_mapdata_init(Map *map, char key[], size_t len, void *value)
{
	MapData *d;

	d = (MapData *) malloc(sizeof(MapData) + len);
	if (d == NULL) {
		painc(1, "_do_mapdata_init malloc error");
	}

	d->h = hash;
	d->klen = len;
	memcopy(&d->key, key, len);

	d->value = value;

	d->next = NULL;
	d->prev = NULL;
	d->listNext = NULL;
	d->listPreve = NULL;
}

static inline void _do_mapdata_add(Map *map, MapData *d)
{
	int i;
	i = d->h % map->cap;

	d->next = map->data[i];
	map->data[i]->prev = d;

	map->data[i] = d;


	d->listNext = map->next;
	map->next->listPrev = d;

	map->next = d;
}

static inline MapData *_do_mapdata_get(Map *map, uint32_t h, char key[], size_t len)
{
	int i;
	i = h % map->cap;

	d = map->data[i];

	while (d != NULL) {
		if ((len == d->klen) && (memcmp(d->key, key, len) == 0)) {
			return d;
		}

		d = d->next;
	}

	return NULL;
}

static inline void _do_mapdata_del(Map *map, MapData *d)
{
	int i;
	i = d->h % map->cap;

	if (d->prev != NULL) {
		d->prev->next = d->next;
		d->listPrev->listNext = d->listNext;
	}

	if (d->next != NULL) {
		d->next->prev = d->prev;
		d->listNext->listPrev = d->listPrev;
	}

	if (map->data[i] == d) {
		map->data[i] = d->next;
	}

	if (map->next == d) {
		map->next = d->next;
	}
}

