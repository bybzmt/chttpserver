#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "buffer.h"

int main()
{
	char data[] = "abcdef1234567890\r\n";
	char trash[10];
	ssize_t re, offset, pos;
	FILE *fp;
	int fd;
	void *ptr;

	struct buffer buf;

	fp = tmpfile();
	if (fp == NULL) {
		printf("tmpfile errno:%d\n", errno);
		exit(1);
	}

	fd = fileno(fp);

	printf("init ok.\n");

	for (int i=0; i<1024*1024; i++) {
		re = fwrite(data, sizeof(data), 1, fp);
		if (re == -1) {
			printf("buf_write errno:%d\n", errno);
			exit(1);
		}
	}

	printf("write ok.\n");

	if (fseek(fp, 0, SEEK_SET) == -1) {
		printf("lseek errno:%d\n", errno);
		exit(1);
	}
	fflush(fp);

	offset = 0;

	buffer_init(&buf);
	ptr = NULL;
	pos = 0;

	for (;;) {
		//printf("for\n");
		re = buffer_push_from_read(&buf, fd);

		//printf("read\n");
		if (re == 0) {
			if (buf.len == buf.cap) {
				printf("error: buffer full!\n");
				exit(1);
			}
			break;
		} else if (re == -1) {
			printf("buf_read errno:%d\n", errno);
			exit(1);
		} else {
			ptr = buffer_chrs(&buf, pos, "\r\n");

			//printf("pos:%ld, %ld\n", offset, pos);
			if (ptr != NULL) {
				re = buffer_shift(&buf, trash, ptr, 10);
				if (re == -1) {
					printf("buf_read errno:%d\n", errno);
				}

				buffer_shift_trash(&buf, 1);

				printf("find:%c, %d\n", ptr, re);
				
				pos = 0;
			} else {
				pos = buf.len;
				printf("chunk:%ld not find\n", pos);
			}
		}
	}

	fclose(fp);

	buffer_destroy(&buf);

	printf("end\n");

	return 0;
}
