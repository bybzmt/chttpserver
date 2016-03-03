#include <assert.h>
#include "mystring.h"

// 类似memchr，但查找字符串中的任意一个字符
// chs是一个字符串以\0结束
void *memchrs(const void *buf, const char *chs, size_t count)
{
	assert(buf != NULL);
	assert(chs != NULL);

	while (count--) {
		for (char *ch=chs; *ch != '\0', ch++) {
			if (*ch == *(char *)buf) {
				return (void *)buf;
			}
		}
		buf++;
	}

	return NULL;
}
