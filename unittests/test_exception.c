#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "exception.h"

void do_panic(void *data)
{
	panic(1, "do_panic");
}

void *thead_main(void *data)
{
	int re;
	Exception e;

	exception_init(&e);

	re = sigsetjmp(e.jmpbuf, 0);
	if (re == 0) {
		do_panic(data);
	} else {
		printf("exception_recover: %p\n", data);
	}

	exception_destroy(&e);

	return NULL;
}

int main()
{
	pthread_t tid, tid2;
	int re;

	exception_key_init();

	re = pthread_create(&tid, NULL, thead_main, (void *)1);
	if (re != 0) {
		printf("pthread_create errno:%d\n", re);
		exit(1);
	}
	
	re = pthread_create(&tid2, NULL, thead_main, (void *)2);
	if (re != 0) {
		printf("pthread_create errno:%d\n", re);
		exit(1);
	}

	pthread_join(tid, NULL);
	pthread_join(tid2, NULL);

	printf("end\n");
	
	return 0;
}
