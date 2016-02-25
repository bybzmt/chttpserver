#include <stdint.h>
#include <setjmp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "exception.h"

void exception_key_init()
{
	int n;
	n = pthread_key_create(&key_exception, NULL);
	if (n != 0) {
		fprintf(stderr, "pthread_key_create error:%d\n", errno);
		exit(1);
	}
}

void panic(int32_t no, const char err[])
{
	Exception *e;

	e = (Exception *)pthread_getspecific(key_exception);

	if (err != NULL) {
		fprintf(stderr, "exception errno:%d error:%s\n", no, err);
	} else {
		fprintf(stderr, "exception errno:%d", no);
	}

	if (e != NULL) {
		//跳出
		siglongjmp(e->jmpbuf, 1);
	} else {
		exit(1);
	}
}

void exception_init(Exception *e2)
{
	Exception *e1;
	int re;

	e1 = (Exception *)pthread_getspecific(key_exception);
	if (e1 == NULL) {
		e2->parent = NULL;
	} else {
		e2->parent = e1;
	}

	re = pthread_setspecific(key_exception, e2);
	if (re != 0) {
		fprintf(stderr, "pthread_setspecific error!\n");
		exit(1);
	}
}

void exception_destroy(Exception *e)
{
	int re;

	if (e->parent == NULL) {
		re = pthread_setspecific(key_exception, NULL);
	} else {
		re = pthread_setspecific(key_exception, e->parent);
	}

	if (re != 0) {
		fprintf(stderr, "pthread_setspecific error!\n");
		exit(1);
	}
}
