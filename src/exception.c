#include <stdint.h>
#include <setjmp.h>
#include <pthread.h>
#include "exception.h"

pthread_key_t key_exception;

typedef struct _exception {
	//父级上下文
	Exception *parent;
	//跳出位置
	sigjmp_buf jmpbuf;
} Exception;

void painc(int32_t no, const char err[])
{
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

	re = pthread_setspecific(key_exception, &e2);
	if (re != 0) {
		fprintf(stderr, "pthread_setspecific error!\n");
		exit(1);
	}
}

void exception_recover(Exception *e)
{
	int re;

	if (e->parent == NULL) {
		n = pthread_setspecific(key_exception, NULL);
	} else {
		n = pthread_setspecific(key_exception, e->parent);
	}

	if (n != 0) {
		fprintf(stderr, "pthread_setspecific error!\n");
		exit(1);
	}

	return re;
}
