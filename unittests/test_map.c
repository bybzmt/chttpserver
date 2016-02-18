#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "map.h"

int main()
{
	Map map;
	MapData *d;
	char *t;
	int i;

	char *data[] = {
		"test1",
		"test2",
		"test3",
		"test4",
		"test5",
		"test6",
		"test7",
		"test8",
		"test9",
		"test10",
		"test11",
		"test12",
		"test13",
		"test14",
		"test15",
		NULL,
	};

	map_init(&map);
	

	for (i=0,t=data[i]; t != NULL; t=data[++i]) {
		map_add(&map, t, strlen(t), t);

		printf("add# cap:%ld/%ld\n", map.len, map.cap);
	}

	d = map_get(&map, data[0], strlen(data[0]));

	printf("%s\n", d->key);

	for (i=0,t=data[i]; t != NULL; t=data[++i]) {
		map_del(&map, t, strlen(t));

		printf("del# cap:%ld/%ld\n", map.len, map.cap);
	}


	map_close(&map);

	return 0;
}
