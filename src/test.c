//#include <pthread.h>
#include <stdlib.h>
#include "socket.h"
#include "wait_signal.h"

int main(int argc, char *argv[])
{
	listen_and_serve();

	wait_signal();

	return 0;
}
