#include "common.h"
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstdint>

uint32_t ceilToPowerOf2(uint32_t num) {
    num += (num == 0); //修复当num等于0时结果为0的边界情况
    num--;
    num |= num >> 1;
    num |= num >> 2;
    num |= num >> 4;
    num |= num >> 8;
    num |= num >> 16;
    num++;
    return num;
}

void assert(bool logicExpr, const char *msg, ...) {
    char buffer[512] = {0};
    va_list args;
    va_start(args, msg); //设置args为msg之后的参数
    vsnprintf(buffer, 512, msg, args);//将可变参数在msg的fmt文本中展开
    va_end(args);
    if (!logicExpr) {
        fprintf(stderr, "%s\n", buffer);
        exit(EXIT_FAILURE);
    }
}
