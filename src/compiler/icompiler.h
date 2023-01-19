//这里是编译器的实现
#ifndef IS_ICOMPILER_H
#define IS_ICOMPILER_H
#include "../lexer/ilexer.h"
#include "../vm/ivm.h"
#include "../utils/iarray.h"
#include "../utils/ibasic.h"
#include "../utils/instream.h"

class CompileUnit {
public:
	Lexer lexer;            //该编译单元所使用的词法分析器
	VM *vm{};               //编译器所服务的虚拟机
	Module *module;         //该编译单元要编译出的模块
	Instream instream;      //当前被编译函数的指令流
	isize scopeDepth{0};    //当前位置的作用域深度
	bool errCurStmt{false}; //标记当前语句是否有错误

	CompileUnit() = default;                 //无参构造
	CompileUnit(const char *file, VM *_vm);  //补全构造

	void compile();     //开始编译

private:
	//工具函数
	Token advance(); //返回当前Token并推进词法分析

	bool matchCTK(TokenKind kind);    //匹配当前Token的类型,匹配则advance,并返回true
	bool matchNTK(TokenKind kind);    //匹配下个Token的类型,匹配则advance两次并返回true
	void assertCTK(TokenKind kind);   //断言当前Token的类型,成功则advance,否则报错

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


#endif //IS_ICOMPILER_H
