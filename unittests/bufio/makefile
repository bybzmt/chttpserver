all: bufio

CC = gcc

#CFLAGS = -Wall -std=gnu99 -pthread -Wformat -Wswitch-default -Wunused -Wparentheses
MY_CFLAGS = -Wall -std=gnu99 -pthread -I../../

CFLAGS = -g $(MY_CFLAGS)

bufio: test_bufio.c bufio.o ../../bufio.c
	$(LINK.c) -o unittest test_bufio.c bufio.o

bufio.o: ../../bufio.c
	$(COMPILE.c) ../../bufio.c

