#ifndef BUFIO_H
#define BUFIO_H

#include "socket.h"

//带缓冲的io
struct bufio {
	//缓冲区容量
	size_t cap;
	//缓冲起始位置
	size_t offset;
	//缓冲
	size_t len;
	//缓冲区指针
	char *buf;
	//文件描术符
	int fd;
};


//从缓冲区读出一行数据，函数以"\r\n"作为行分隔
//成功返回读取到的字节数
//发生错误反回-1
ssize_t buf_readline(struct bufio *buf, const void *data, size_t len);

//从缓冲区读出数据
//成功返回读取到的字节数
//发生错误反回-1
ssize_t buf_read(struct bufio *buf, const void *data, size_t len);

//向缓冲区写入数据，函数总是将数据全布写入缓冲才返回
//返回写入的字节数
//发生错误反回-1
ssize_t buf_write(struct bufio *buf, const void *data, size_t len);

//将所有缓冲写入
//发生错误反回-1
ssize_t buf_flush(struct bufio *buf);


#endif
