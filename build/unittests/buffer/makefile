all: dorun

CC = gcc

#CFLAGS = -Wall -std=gnu99 -pthread -Wformat -Wswitch-default -Wunused -Wparentheses
MY_CFLAGS = -Wall -std=gnu99 -pthread -I../../../include

CFLAGS = -g $(MY_CFLAGS)

dorun: test_buffer.o string.o buffer.o exception.o
	$(LINK.c) -o unittest.out test_buffer.o string.o buffer.o exception.o

test_buffer.o: ../../../unittests/test_buffer.c
	$(COMPILE.c) ../../../unittests/test_buffer.c

buffer.o: ../../../src/buffer.c
	$(COMPILE.c) ../../../src/buffer.c

string.o: ../../../src/string.c
	$(COMPILE.c) ../../../src/string.c

exception.o: ../../../src/exception.c
	$(COMPILE.c) ../../../src/exception.c

clean:
	-rm *.o *.out
