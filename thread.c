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


void thread_create(struct client * client)
{
	pthread_t tid;
	int re;

	re = pthread_create(&tid, NULL, thread_callback, (void *)client);
	if (re != 0) {
		printf("call pthread_create errno:%d\n", re);
		//清理资源
		close(client->connfd);
		free(client);
	}
}

static void *thread_callback(void *client)
{
	//注册清理函数
	pthread_cleanup_push(thread_clear, client);

	//处理连接
	http_serve((struct client *)client);

	//永远执行清理
	pthread_cleanup_pop(1);

	return NULL;
}

static void thread_clear(void *client)
{
	struct client *_client;
	_client = (struct client *)client;

	//关闭连接并清理
	close(_client->connfd);
	free(_client);
}
