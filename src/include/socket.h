#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
//#include <sys/types.h>

struct addr {
	socklen_t len;
	struct sockaddr_in addr;
};

struct _ctx {
	//父级上下文
	//Ctx *parent;
	//跳出位置
	sigjmp_buf jmpbuf;
	int32_t errno;

	int fd;
	struct addr addr;
} Ctx;

void listen_and_serve();

void ctx_close(Ctx *);

#endif
