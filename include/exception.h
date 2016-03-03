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

//初始化异常key
void exception_key_init();

//发生异常
void panic(int32_t no, const char err[]);

//异常初始化
void exception_init(Exception *e2);

//异常销毁
void exception_destroy(Exception *e);

#endif
