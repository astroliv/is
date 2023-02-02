//这里是编译器的实现
#ifndef IS_ICOMPILER_H
#define IS_ICOMPILER_H
#include "../lexer/ilexer.h"
#include "../vm/ivm.h"
#include "../container/iarray.h"
#include "../container/imodifier.h"
#include "../basic/ibasic.h"

class Compiler;

class CompileUnit {
public:
	Lexer lexer;            //该编译单元所使用的词法分析器
	Compiler *com{};        //所属的编译器

	//下面几个状态标记
	isize scopeDepth{0};    //当前位置的作用域深度

	bool validMatch{false}; //记录当前的惰性匹配是否有效,用于判定报错的标记
	bool errCurFile{false}; //标记当前文件是否有错误,若有则不会调用VM执行
	bool errCurStmt{false}; //标记当前语句是否有错误,有则会进行跳过

	CompileUnit() = default;                                 //无参构造
	explicit CompileUnit(const char *file, Compiler *_com);  //补全构造

	void init(const char *file, Compiler *_com);             //补全初始化

	void compile();     //开始编译

private:
	//工具函数

	Token advance(); //返回当前Token并推进词法分析

	bool matchCTK(TokenKind kind);    //匹配当前Token的类型,匹配则advance,并返回true
	bool matchNTK(TokenKind kind);    //匹配下个Token的类型,匹配则advance两次并返回true
	void assertCTK(TokenKind kind);   //断言当前Token的类型,成功则advance,否则只报错
	void skipCurStmt();               //判断若errCurStmt为true,则跳过当前语句,并将其置为false

	//下面是编译用的递归调用链

	//语句
	bool stmts();

	//算数表达式
	bool expr();
	bool term();
	bool factor();
	bool power();
	bool atom();

};

//用于整合调度多文件编译
class Compiler {
public:
	Array<CompileUnit> cuList;  //编译单元列表,每个编译单元仅处理编译一个文件
	ByteStream instream;        //指令流集合,该项目的所有函数指令都在这
	VM *vm{};                   //编译器服务的虚拟机

	Compiler() = default;                 //默认构造
	explicit Compiler(VM *_vm);           //VM构造
	Compiler(const char *file, VM *_vm);  //补全构造

	void init(VM *_vm);                   //VM初始化
	void init(const char *file, VM *_vm); //补全初始化

	void mkCompileUnit(const char *file); //向cuList中加入新的成员,并令其执行编译

	//指令与指令参数的写入

	void write(Bytecode type);                               //写字节码
	void write(int64_t operand, int8_t len);                 //写指定字节操作数
	void write(Bytecode type, int64_t operand, int8_t len);  //写指定字节操作数指令

	void writeVarg(uint64_t arg);   //变长字节码参数的写入
};

#endif //IS_ICOMPILER_H
