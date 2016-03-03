all: dorun

CC = gcc

#CFLAGS = -Wall -std=gnu99 -pthread -Wformat -Wswitch-default -Wunused -Wparentheses
MY_CFLAGS = -Wall -std=gnu99 -pthread -I../../../include

CFLAGS = -g $(MY_CFLAGS)

dorun: test_chunk_buffer.o chunk_data.o chunk_buffer.o exception.o
	$(LINK.c) -o unittest.out test_chunk_buffer.o chunk_data.o chunk_buffer.o exception.o

test_chunk_buffer.o: ../../../unittests/test_chunk_buffer.c
	$(COMPILE.c) ../../../unittests/test_chunk_buffer.c

chunk_data.o: ../../../src/chunk_data.c
	$(COMPILE.c) ../../../src/chunk_data.c

chunk_buffer.o: ../../../src/chunk_buffer.c
	$(COMPILE.c) ../../../src/chunk_buffer.c

exception.o: ../../../src/exception.c
	$(COMPILE.c) ../../../src/exception.c

clean:
	-rm *.o *.out
