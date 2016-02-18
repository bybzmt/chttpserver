#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <setjmp.h>
#include <pthread.h>

pthread_key_t key_exception;

typedef struct _exception {
	//父级上下文
	Exception *parent;
	//跳出位置
	sigjmp_buf jmpbuf;
} Exception;

void painc(int32_t no, const char err[]);

void exception_init(Exception *e2);

void exception_recover(Exception *e);

#endif
