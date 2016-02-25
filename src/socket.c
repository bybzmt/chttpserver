#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

#include "socket.h"
#include "thread.h"

void listen_and_serve()
{
	int listen_fd, re, accept_fd;
	struct sockaddr_in servaddr;

	Ctx *ctx;

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(8080);

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd == -1) {
		printf("call socket errno:%d\n", errno);
		exit(1);
	}

	re = bind(listen_fd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (re == -1) {
		printf("call bind errno:%d\n", errno);
		exit(1);
	}

	re = listen(listen_fd, 1024);
	if (re == -1) {
		printf("call listen errno:%d\n", errno);
		exit(1);
	}

	for (;;) {
		ctx = (Ctx *)malloc(sizeof(Ctx));
		if (ctx == NULL) {
			printf("call malloc errno:%d\n", errno);
			exit(1);
		}
		ctx->addr.len = sizeof(struct sockaddr_in);

		ctx->fd = accept(listen_fd, (struct sockaddr *)&ctx->addr.addr, &ctx->addr.len);
		if (ctx->fd  == -1) {
			printf("call accept errno:%d\n", errno);
			free(ctx);
		} else {
			thread_create(ctx);
		}
	}
}

void ctx_close(Ctx *ctx)
{
	close(ctx->fd);
	free(ctx);
}

