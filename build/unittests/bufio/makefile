all: bufio

CC = gcc

#CFLAGS = -Wall -std=gnu99 -pthread -Wformat -Wswitch-default -Wunused -Wparentheses
MY_CFLAGS = -Wall -std=gnu99 -pthread -I../../src/include

CFLAGS = -g $(MY_CFLAGS)

bufio: test_bufio.o bufio.o
	$(LINK.c) -o unittest.out test_bufio.o bufio.o

test_bufio.o: test_bufio.c
	$(COMPILE.c) test_bufio.c

bufio.o: ../../src/bufio.c
	$(COMPILE.c) ../../src/bufio.c

clean:
	-rm *.o *.out
