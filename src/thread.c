#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "thread.h"
#include "http.h"

//线程运行
static void *thread_callback(void *client);

//线程结速时清理函数
static void thread_clear(void *client);

void thread_create(Ctx *ctx)
{
	pthread_t tid;
	int re;

	re = pthread_create(&tid, NULL, thread_callback, (void *)ctx);
	if (re != 0) {
		printf("call pthread_create errno:%d\n", re);

		//清理资源
		ctx_close(ctx);
	}
}

void *thread_callback(void *ctx)
{
	//注册清理函数
	pthread_cleanup_push(thread_clear, ctx);

	//处理连接
	http_serve((Ctx *)ctx);

	//永远执行清理
	pthread_cleanup_pop(1);

	return NULL;
}

void thread_clear(void *ctx)
{
	ctx_close((Ctx *) ctx);
}

