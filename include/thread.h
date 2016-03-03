#ifndef THREAD_H
#define THREAD_H

#include <setjmp.h>
#include "base.h"
#include "socket.h"

//创建线程
void thread_create(Ctx *);

#endif
