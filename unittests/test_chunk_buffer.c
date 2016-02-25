#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "chunk_buffer.h"
#include "chunk_data.h"

int main()
{
	struct chunk_buffer buf;
	char data[] = "abcdef1234567890\n";
	ssize_t re, offset, pos;
	FILE *fp;
	int fd;
	struct chunk_data *tmp;

	chunk_buffer_init(&buf);

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

	for (;;) {
		//printf("for\n");
		re = chunk_buffer_readMore(&buf, fd);
		//printf("read\n");
		if (re == 0) {
			break;
		} else if (re == -1) {
			printf("buf_read errno:%d\n", errno);
			exit(1);
		} else {
			pos = chunk_buffer_pos(&buf, offset, '\n');
			//printf("pos:%ld, %ld\n", offset, pos);
			if (pos != -1) {
				tmp = chunk_buffer_to_chunk_data(&buf, offset, pos-offset+1);
				//printf("chunk:%p\n", tmp);

				offset += re;

				if (tmp != NULL) {
					//printf("chunk:%ld len:%ld chunck_num:%d\n", pos, tmp->len, tmp->chunk_num);
					free(tmp);
				} else {
					exit(1);
				}
			} else {
				printf("chunk:%ld not find\n", pos);
			}
		}
	}

	fclose(fp);

	printf("chunk_buffer len:%ld chunk_size:%d, chunk_num:%d, chunk_slot:%d\n", buf.len, buf.chunk_size, buf.chunk_num, buf.chunk_slot);
	chunk_buffer_destroy(&buf);

	return 0;
}
