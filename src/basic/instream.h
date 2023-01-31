////指令流,这是一个载装了Array<byte>的工具类,可以用于快捷地读写字节码与参数(现已废弃)
//#ifndef IS_INSTREAM_H
//#define IS_INSTREAM_H
//
//#include "../container/iarray.h"
//#include "../string/istring.h"
//#include "ibytecode.h"
//
//class Instream {
//public:
//	ByteStream *byteStream{};   //当前使用的字节流
//	uint8_t advLen{0};          //读出的操作数的字节数(Advance Length)
//
//	Instream() = default;                 //无参构造
//	explicit Instream(ByteStream *bst);   //补全构造
//
//	//下面是一些工具函数
//
//	void write(Bytecode type) const;                               //写字节码
//
//	void write(byte operand) const;                                //写单字节操作数
//	void write(Bytecode type, byte operand) const;                 //写单字节操作数指令
//
//	void write(int64_t operand, int8_t len) const;                 //写指定字节操作数
//	void write(Bytecode type, int64_t operand, int8_t len) const;  //写指定字节操作数指令
//
//	int64_t read(isize idx);                                       //读idx处指令操作数
//	int64_t read(isize idx, int8_t operandLen);                    //读idx处指令指定字节操作数
//
//	void writeVarg(uint64_t arg) const;                            //变长字节码参数的写入
//	uint64_t readVarg(isize idx);                                  //变长字节码参数的读取
//
//	string dump(isize idx);                                        //输出idx处的指令文本
//};
//
//
//#endif //IS_INSTREAM_H
