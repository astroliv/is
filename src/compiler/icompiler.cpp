#include "icompiler.h"
#include "../report/ireport.h"

//对于会被多次使用的类成员变量和语句就用宏来简化

#define PTK lexer.preToken.kind
#define PT (lexer.preToken)

#define CTK lexer.curToken.kind
#define CT (lexer.curToken)

#define NTK lexer.nexToken.kind
#define NT (lexer.nexToken)

#define STK lexer.secToken.kind
#define ST (lexer.secToken)

#define TTK lexer.thiToken.kind
#define TT (lexer.thiToken)


//惰性匹配,匹配失败就摆烂返回false(其实就是CaseFalseReturnFalse)
#define inertMatch(fn) \
if(!(fn)) { return validMatch = false; }

//强制匹配,匹配失败就开摆,报错返回一气呵成
//(为了防止回到Caller的时候因返回false重复报错,搞了个errCurStmt来判定)
#define forceMatch(fn, id, ...)         \
if (!(fn)) {                            \
    if (!errCurStmt) {                  \
    errCurFile = errCurStmt = true;     \
    reportMsg(id, this,##__VA_ARGS__);  \
    }                                   \
    return validMatch = false;          \
}

//竭力匹配,竭力匹配完当前语句,即使有失败也不放弃,报个错接着匹配(该语句是以sign结尾)
#define striveMatch(fn, sign, id, ...)  \
while (!(fn)){                          \
    errCurFile = errCurStmt = true;     \
    reportMsg(id, this,##__VA_ARGS__);  \
    if (CTK != sign) { advance(); }     \
    else { break; }                     \
}


bool cmpFstrOfValue(const Value &a, const Value &b);//比较俩Value的fstr数据是否相等

CompileUnit::CompileUnit(const char *file, VM *_vm) : vm(_vm) {
	lexer.init(file);
	module = new Module();//以后会换成~compiler.moduleList
	module->file = file;
	instream.byteStream = new ByteStream(16);

}

void CompileUnit::compile() {
	stmts();
}

inline Token CompileUnit::advance() {
	lexer.advance();
	return PT;
}

inline bool CompileUnit::matchNTK(TokenKind kind) {
	if (NTK != kind) { return false; }
	advance();  //Pass CurToken
	advance();  //Pass NexToken
	return true;
}

inline bool CompileUnit::matchCTK(TokenKind kind) {
	if (CTK != kind) { return false; }
	advance();
	return true;
}

inline void CompileUnit::assertCTK(TokenKind kind) {
	if (CTK != kind) {
		reportMsg(RepId::assertTokenKindFailure, this,
		          tokenKindName[(uint16_t) kind], tokenKindName[(uint16_t) CTK]);
	}
	advance();//Pass CurToken
}

bool CompileUnit::stmts() {
	while (CTK != TokenKind::eof) {
		if (!expr()) { errCurFile = true; }//目前情况下所有的语句只能是表达式,若不是则错误

		if (errCurStmt) {//跳到当前语句的结束位置
			while (CTK != TokenKind::end && CTK != TokenKind::eof) { advance(); }
			errCurStmt = false;
		}
		assertCTK(TokenKind::end);
	}
	instream.write(Bytecode::end);
	return validMatch = true;
}


bool CompileUnit::expr() {
	inertMatch(term());
	while (true) {//若匹配到该循环内的任何分支,则必定是强制匹配,若匹配失败则报错
		if (matchCTK(TokenKind::add)) {//编译加法
			striveMatch(term(), TokenKind::end,
			            RepId::expectedExpr, tokenKindName[(isize) CTK]);
			instream.write(Bytecode::add);
		} else if (matchCTK(TokenKind::sub)) {//编译减法
			striveMatch(term(), TokenKind::end,
			            RepId::expectedExpr, tokenKindName[(isize) CTK]);
			instream.write(Bytecode::sub);
		} else { break; }
	}
	return validMatch = true;
}

bool CompileUnit::term() {
	inertMatch(factor());
	while (true) {//若匹配到该循环内的任何分支,则必定是强制匹配,若匹配失败则报错
		if (matchCTK(TokenKind::mul)) {//编译乘法,强制匹配
			striveMatch(factor(), TokenKind::end,
			            RepId::expectedExpr, tokenKindName[(isize) CTK]);
			instream.write(Bytecode::mul);
		} else if (matchCTK(TokenKind::div)) {//编译除法
			striveMatch(factor(), TokenKind::end,
			            RepId::expectedExpr, tokenKindName[(isize) CTK]);
			instream.write(Bytecode::div);
		} else if (matchCTK(TokenKind::mod)) {//编译取余
			striveMatch(factor(), TokenKind::end,
			            RepId::expectedExpr, tokenKindName[(isize) CTK]);
			instream.write(Bytecode::mod);
		} else { break; }
	}
	return validMatch = true;
}

bool CompileUnit::factor() {
	if (matchCTK(TokenKind::add)) {//取本身
		striveMatch(factor(), TokenKind::end,
		            RepId::expectedExpr, tokenKindName[(isize) CTK]);
	} else if (matchCTK(TokenKind::sub)) {
		striveMatch(factor(), TokenKind::end,
		            RepId::expectedExpr, tokenKindName[(isize) CTK]);
		instream.write(Bytecode::neg);//取相反数
	} else {
		inertMatch(power());
	}
	return validMatch = true;
}

bool CompileUnit::power() {
	inertMatch(atom());
	while (true) {//若匹配到该循环内的任何分支,则必定是强制匹配,若匹配失败则报错
		if (matchCTK(TokenKind::pow)) {//编译幂运算
			striveMatch(factor(), TokenKind::end,
			            RepId::expectedExpr, tokenKindName[(isize) CTK]);
			instream.write(Bytecode::pow);
		} else { break; }
	}
	return validMatch = true;
}

bool CompileUnit::atom() {
	if (matchCTK(TokenKind::num)) {//编译数值字面量
		instream.write(Bytecode::ldc, 0, 4);
		instream.write(module->constList.reg(PT.value), 4);
	} else if (matchCTK(TokenKind::str)) {//编译字符串字面量
		instream.write(Bytecode::ldc, 0, 4);
		instream.write(module->constList.reg(PT.value, &cmpFstrOfValue), 4);
	} else if (matchCTK(TokenKind::lpare)) {//括号包含的表达式
		striveMatch(expr(), TokenKind::rpare,
		            RepId::expectedExpr, tokenKindName[(isize) CTK]);
		assertCTK(TokenKind::rpare);
	} else {
		return validMatch = false;
	}
	return validMatch = true;
}

bool cmpFstrOfValue(const Value &a, const Value &b) {
	//由于Array<T>内查找与注册等方法的比较都是直接作==运算,那么对于Value类型,就要定义一个==的操作符重载,才能
	//使用Array<Value>.为了普适性,我为Value定义的是直接进行内容比较.这样的话要是比较字符串,那就相当于直接比较
	//指针,这肯定不是我们想要的结果.于是我为Array<T>设计了一个比较的方式,可以指定用于比较的函数,然后使用该函数
	//比较,由此进行查找,注册等方法.于是在我们要比较字符串内容时,只需要指定这个函数,便可以解决其只会比较指针的问题
	return *a.fsp == *b.fsp;
}

