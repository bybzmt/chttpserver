all: dorun

CC = gcc

#CFLAGS = -Wall -std=gnu99 -pthread -Wformat -Wswitch-default -Wunused -Wparentheses
MY_CFLAGS = -Wall -std=gnu99 -pthread -I../../../src/include

CFLAGS = -g $(MY_CFLAGS)

dorun: test_map.o map.o exception.o hash.o base.o
	$(LINK.c) -o unittest.out test_map.o map.o exception.o hash.o base.o

test_map.o: ../../../unittests/test_map.c
	$(COMPILE.c) ../../../unittests/test_map.c

map.o: ../../../src/map.c
	$(COMPILE.c) ../../../src/map.c

exception.o: ../../../src/exception.c
	$(COMPILE.c) ../../../src/exception.c

hash.o: ../../../src/hash.c
	$(COMPILE.c) ../../../src/hash.c

base.o: ../../../src/base.c
	$(COMPILE.c) ../../../src/base.c

clean:
	-rm *.o *.out
