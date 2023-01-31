//这里是编译器的实现
#ifndef IS_ICOMPILER_H
#define IS_ICOMPILER_H
#include "../lexer/ilexer.h"
#include "../vm/ivm.h"
#include "../container/iarray.h"
#include "../container/imodifier.h"
#include "../basic/ibasic.h"
#include "../basic/instream.h"

class CompileUnit {
public:
	Lexer lexer;            //该编译单元所使用的词法分析器
	VM *vm{};               //编译器所服务的虚拟机

	//下面几个状态标记
	isize scopeDepth{0};    //当前位置的作用域深度

	bool validMatch{false}; //记录当前的惰性匹配是否有效,用于判定报错的标记
	bool errCurFile{false}; //标记当前文件是否有错误,若有则不会调用VM执行
	bool errCurStmt{false}; //标记当前语句是否有错误,有则会进行跳过

	CompileUnit() = default;                 //无参构造
	CompileUnit(const char *file, VM *_vm);  //补全构造

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


#endif //IS_ICOMPILER_H
