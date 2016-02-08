all: test

CC = gcc

#CFLAGS = -Wall -std=gnu99 -pthread -Wformat -Wswitch-default -Wunused -Wparentheses
MY_CFLAGS = -Wall -std=gnu99 -pthread

# options for development
CFLAGS = -g $(MY_CFLAGS)
# options for release
#CFLAGS = -O $(MY_CFLAGS)

#make default rule
#LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)
#COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

test: test.o wait_signal.o socket.o thread.o http.o
	$(LINK.c) -o test test.o wait_signal.o socket.o thread.o http.o

test.o: test.c
	$(COMPILE.c) test.c

wait_signal.o: wait_signal.c
	$(COMPILE.c) wait_signal.c

socket.o: socket.c
	$(COMPILE.c) socket.c

thread.o: thread.c
	$(COMPILE.c) thread.c

http.o: http.c
	$(COMPILE.c) http.c

clean:
	-rm test
	-rm *.o
	-rm *.gch

install:
	@echo "no have install script."
