#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
//#include <sys/types.h>

//客户端连信息
struct client {
	int connfd;
	socklen_t len;
	struct sockaddr_in addr;
};

void listen_and_serve();

#endif
