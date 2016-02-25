all: dorun

CC = gcc

#CFLAGS = -Wall -std=gnu99 -pthread -Wformat -Wswitch-default -Wunused -Wparentheses
MY_CFLAGS = -Wall -std=gnu99 -pthread -I../../../src/include

CFLAGS = -g $(MY_CFLAGS)

dorun: test_exception.o exception.o
	$(LINK.c) -o unittest.out test_exception.o exception.o

test_exception.o: ../../../unittests/test_exception.c
	$(COMPILE.c) ../../../unittests/test_exception.c

exception.o: ../../../src/exception.c
	$(COMPILE.c) ../../../src/exception.c

clean:
	-rm *.o *.out
