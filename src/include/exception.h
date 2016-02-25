#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <setjmp.h>
#include <stdint.h>
#include <pthread.h>

pthread_key_t key_exception;

typedef struct _exception {
	//父级上下文
	struct _exception *parent;
	//跳出位置
	sigjmp_buf jmpbuf;
} Exception;

void panic(int32_t no, const char err[]);

void exception_init(Exception *e2);

void exception_recover(Exception *e);

#endif
