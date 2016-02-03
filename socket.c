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
	int listenfd, re;
	struct sockaddr_in servaddr;

	struct client *client;

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(8080);

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1) {
		printf("call socket errno:%d\n", errno);
		exit(1);
	}

	re = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (re == -1) {
		printf("call bind errno:%d\n", errno);
		exit(1);
	}

	re = listen(listenfd, 1024);
	if (re == -1) {
		printf("call listen errno:%d\n", errno);
		exit(1);
	}

	for (;;) {
		client = (struct client *)malloc(sizeof(struct client));
		if (client == NULL) {
			printf("call malloc errno:%d\n", errno);
			exit(1);
		}
		client->len = sizeof(struct sockaddr_in);

		client->connfd = accept(listenfd, (struct sockaddr *)&client->addr, &client->len);
		if (client->connfd == -1) {
			printf("call accept errno:%d\n", errno);
			free(client);
		} else {
			thread_create(client);
		}
	}
}

