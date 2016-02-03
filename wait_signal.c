#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "wait_signal.h"

void wait_signal()
{
	int err, signo;
	sigset_t mask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);
	sigaddset(&mask, SIGHUP);

	err = pthread_sigmask(SIG_BLOCK, &mask, NULL);
	if (err != 0) {
		printf("SIG_BLOCK error:%d", err);
		exit(1);
	}

	for (;;) {
		err = sigwait(&mask, &signo);
		if (err != 0) {
			printf("sigwait error:%d", err);
			exit(1);
		}

		switch (signo) {
			case SIGINT:
				printf("signal %d\n", signo);
				break;

			case SIGQUIT:
				printf("signal %d\n", signo);
				exit(0);
				break;

			case SIGHUP:
				printf("signal %d\n", signo);
				exit(0);
				break;

			default:
				printf("unexpected signal %d\n", signo);
				exit(1);
		}
	}
}
