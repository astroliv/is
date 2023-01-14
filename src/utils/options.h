//提供一些可供调控的编译设置选项
#ifndef IS_OPTIONS_H
#define IS_OPTIONS_H

//下面这些是debug选项

//调试模式,可以帮助你快速找到编译器代码出错的原因
#define DEBUG_MODE

//调试模式下是否使用宏定义的断言,这种断言可以让你直接找到断言失败代码段的位置
#define DEBUG_MODE_USING_MICRO_ASSERT

//调试断言 的输出文本缓存区大小
#define DEBUG_ASSERT_BUFFER_SIZE 512

//调试断言 断言失败是否直接关闭
#define DEBUG_ASSERT_CLOSE false


#endif //IS_OPTIONS_H
