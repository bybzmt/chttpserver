all: test

CC = gcc

#CFLAGS = -Wall -std=gnu99 -pthread -Wformat -Wswitch-default -Wunused -Wparentheses
MY_CFLAGS = -Wall -std=gnu99 -pthread -I../src/include/

# options for development
CFLAGS = -g $(MY_CFLAGS)
# options for release
#CFLAGS = -O $(MY_CFLAGS)

#make default rule
#LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)
#COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

test: test.o wait_signal.o socket.o thread.o http.o
	$(LINK.c) -o test.out test.o wait_signal.o socket.o thread.o http.o

test.o: ../src/test.c
	$(COMPILE.c) ../src/test.c

wait_signal.o: ../src/wait_signal.c ../src/include/wait_signal.h
	$(COMPILE.c) ../src/wait_signal.c

socket.o: ../src/socket.c ../src/include/socket.h
	$(COMPILE.c) ../src/socket.c

thread.o: ../src/thread.c ../src/include/thread.h
	$(COMPILE.c) ../src/thread.c

http.o: ../src/http.c ../src/include/http.h
	$(COMPILE.c) ../src/http.c

clean:
	-rm *.o *.out

install:
	@echo "no have install script."
