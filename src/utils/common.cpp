#include "common.h"

isize ceilToPowerOf2(isize num) {
    num += (num == 0); //修复当num等于0时结果为0的边界情况
    num--;
    num |= num >> 1;
    num |= num >> 2;
    num |= num >> 4;
    num |= num >> 8;
    num |= num >> 16;
//    num |= num >> 32; //若要将isize改为ui64,则应取消该语句的注释
    num++;
    return num;
}

#ifdef DEBUG_MODE
#ifndef DEBUG_MODE_USING_MICRO_ASSERT

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

void assert(bool logicexpr, const char *msg, ...) {
    char buffer[DEBUG_ASSERT_BUFFER_SIZE] = {0};
    va_list args;
    va_start(args, msg); //设置args为msg之后的参数
    vsnprintf(buffer, DEBUG_ASSERT_BUFFER_SIZE, msg, args);//将可变参数在msg的fmt文本中展开
    va_end(args);
    if (!logicexpr) {
//        fprintf(stderr, "%s\n", buffer);
        printf("\033[0m\033[1;31m[Code Error]Assert failed: %s\033[0m\n", buffer);
        if (DEBUG_ASSERT_CLOSE) { exit(EXIT_FAILURE); }
    }
}

#endif
#endif
