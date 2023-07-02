//这里放的是一些基础的工具函数/宏,和类型定义
#ifndef IS_COMMON_H
#define IS_COMMON_H

#include "ioptions.h"
#include <cstdint>

using isize = uint32_t; //本程序所有的索引所使用的类型固定为ui32
using byte = uint8_t;   //本程序所有的字节所使用的类型固定为ui8


isize ceilToPowerOf2(isize num);    //将输入数字向上对齐到2的整次幂

#define MAX(a, b) ((a) > (b) ? (a) : (b) )
#define MIN(a, b) ((a) < (b) ? (a) : (b) )

#ifdef DEBUG_MODE

#ifndef DEBUG_MODE_USING_MICRO_ASSERT

//断言,即判断logicExpr是否为false,若为false则弹出错误信息msg(msg支持c-fmt)并退出程序,否则不进行操作
void assert(bool logicexpr, const char *msg, ...);

#else

#include <cstdlib>
#include <cstdio>

#define assert(logicexpr, msg, ...)\
    do\
    {\
        if (!(logicexpr))\
        {\
            char buffer[DEBUG_ASSERT_BUFFER_SIZE] = {0};\
            sprintf(buffer,msg,##__VA_ARGS__);\
            printf("\033[0m\033[1;31mcode error: <assert failed> in file:\n%s:%d function: %s(...): %s\033[0m\n",\
                    __FILE__, __LINE__, __FUNCTION__, buffer);                                                  \
        if(DEBUG_ASSERT_CLOSE){exit(EXIT_FAILURE);}\
        }\
    } while (0);
#endif

#else
#define assert(logicExpr, msg, ...) ;
#endif

#endif //IS_COMMON_H
