#include <stdint.h>
#include "exception.h"

void base_init()
{
	int n;
	n = pthread_key_create(key_exception, NULL);
	if (n != 0) {
		fprintf(stderr, "pthread_key_create error:%d\n", errno);
		exit(1);
	}
}


