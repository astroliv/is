#ifndef IS_COMMON_H
#define IS_COMMON_H

#include <cstdint>

//将输入数字向上对齐到2的整次幂
uint32_t ceilToPowerOf2(uint32_t num);

//断言,即判断logicExpr是否为false,若为false则弹出错误信息msg(msg支持c-fmt)并退出程序,否则不进行操作
void assert(bool logicExpr, const char *msg, ...);

#endif //IS_COMMON_H
