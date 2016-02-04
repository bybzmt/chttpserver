#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "bufio.h"

int main()
{
	struct bufio buf;
	char data[] = "abcdef1234567890\n";
	char data2[13];
	ssize_t re;
	FILE *fp;
	off_t re2;
	int i;

	memset(&buf, 0, sizeof(buf));
	buf.cap = 1024;
	buf.buf = (char *)malloc(buf.cap);
	if (buf.buf == NULL) {
		printf("mkstemp errno:%d\n", errno);
		exit(1);
	}

	fp = tmpfile();
	if (fp == NULL) {
		printf("tmpfile errno:%d\n", errno);
		exit(1);
	}

	buf.fd = fileno(fp);

	printf("init ok.\n");

	for (i=0; i<1024; i++) {
		re = buf_write(&buf, data, sizeof(data));
		if (re == -1) {
			printf("buf_write errno:%d\n", errno);
			exit(1);
		}
	}

	printf("write ok.\n");

	re2 = lseek(buf.fd, 0, SEEK_SET);
	if (re2 == -1) {
		printf("lseek errno:%d\n", errno);
		exit(1);
	}

	for (;;) {
		re = buf_read(&buf, data2, sizeof(data2));
		if (re == 0) {
			break;
		} else if (re == -1) {
			printf("buf_read errno:%d\n", errno);
			exit(1);
		} else {
			fputs("buf_read:[", stdout);
			fwrite(data2, 1, re, stdout);
			fputs("]\n", stdout);
		}
	}

	printf("read ok.\n");

	re2 = lseek(buf.fd, 0, SEEK_SET);
	if (re2 == -1) {
		printf("lseek errno:%d\n", errno);
		exit(1);
	}

	for (;;) {
		re = buf_readline(&buf, data2, sizeof(data2));
		if (re == 0) {
			break;
		} else if (re == -1) {
			printf("buf_readline errno:%d\n", errno);
			exit(1);
		} else {
			fputs("buf_readline:[", stdout);
			fwrite(data2, 1, re, stdout);
			fputs("]\n", stdout);
		}
	}

	printf("readline ok.\n");

	return 0;
}
