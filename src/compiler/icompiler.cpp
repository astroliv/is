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

//一个简化代码用的宏 (Cur Token Not Equal End & Rpare)
#define CTKNEER CTK != TokenKind::end && CTK != TokenKind::rpare

//下面是几个宏,用于处理匹配报错的

//惰性匹配,匹配失败就摆烂返回false(其实就是CaseFalseReturnFalse)
#define inertMatch(fn) \
if(!(fn)) { return validMatch = false; }

//强制匹配,匹配失败就开摆,报错返回一气呵成
//(为了防止回到Caller的时候因返回false重复报错,搞了个errCurStmt来判定)
#define forceMatch(fn, id, ...)             \
if (!(fn)) {                                \
    if (!errCurStmt) {                      \
    reportMsg(id, this,##__VA_ARGS__);      \
    }                                       \
    return validMatch = false;              \
}

//竭力匹配,竭力匹配完当前语句,即使有失败也不放弃,报个错接着匹配,若sign为false则停止
#define striveMatch(fn, sign, id, ...)      \
while (!(fn) && CTK != TokenKind::eof) {    \
    errCurFile = errCurStmt = true;         \
    reportMsg(id, this,##__VA_ARGS__);      \
    if (sign) { advance(); }                \
    else { break; }                         \
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
	advance();  //Pass curToken
	advance();  //Pass nexToken
	return true;
}

inline bool CompileUnit::matchCTK(TokenKind kind) {
	if (CTK != kind) { return false; }
	advance();//Pass curToken
	return true;
}

inline void CompileUnit::assertCTK(TokenKind kind) {
	if (CTK != kind) {
		reportMsg(RepId::assertTokenKindFailure, this,
		          tokenKindName(kind), tokenKindName(CTK));
		return;//由于当前token不是目前预期的,那么就认为其为下一个匹配所需的,不应被Pass掉
	}
	advance();//Pass curToken
}

inline void CompileUnit::skipCurStmt() {
	if (!errCurStmt) { return; }
	while (CTK != TokenKind::end && CTK != TokenKind::eof) { advance(); }
	errCurStmt = false;
}

bool CompileUnit::stmts() {
	while (CTK != TokenKind::eof) {
		if (!expr()) { errCurFile = true; }
		skipCurStmt();
		if (lexer.unLpare || lexer.unLbracket || lexer.unLbrace) {
			reportMsg(RepId::unmatchedBracketsStmt, this);
		}//这只是一个提示,因为实际上assertTokenKindFailure也会对该情况进行报错
		lexer.unLpare = lexer.unLbracket = lexer.unLbrace = 0;
		assertCTK(TokenKind::end);
	}
	vm->write(Bytecode::end);
	return validMatch = true;
}


bool CompileUnit::expr() {
	inertMatch(term());
	while (true) {
		if (matchCTK(TokenKind::add)) {
			striveMatch(term(), CTKNEER, RepId::expectedExpr, tokenKindName(CTK));
			vm->write(Bytecode::add);
		} else if (matchCTK(TokenKind::sub)) {
			striveMatch(term(), CTKNEER, RepId::expectedExpr, tokenKindName(CTK));
			vm->write(Bytecode::sub);
		} else { break; }
	}
	return validMatch = true;
}

bool CompileUnit::term() {
	inertMatch(factor());
	while (true) {
		if (matchCTK(TokenKind::mul)) {
			striveMatch(factor(), CTKNEER, RepId::expectedExpr, tokenKindName(CTK));
			vm->write(Bytecode::mul);
		} else if (matchCTK(TokenKind::div)) {
			striveMatch(factor(), CTKNEER, RepId::expectedExpr, tokenKindName(CTK));
			vm->write(Bytecode::div);
		} else if (matchCTK(TokenKind::mod)) {
			striveMatch(factor(), CTKNEER, RepId::expectedExpr, tokenKindName(CTK));
			vm->write(Bytecode::mod);
		} else { break; }
	}
	return validMatch = true;
}

bool CompileUnit::factor() {
	if (matchCTK(TokenKind::add)) {
		striveMatch(factor(), CTKNEER, RepId::expectedExpr, tokenKindName(CTK));
	} else if (matchCTK(TokenKind::sub)) {
		striveMatch(factor(), CTKNEER, RepId::expectedExpr, tokenKindName(CTK));
		vm->write(Bytecode::neg);
	} else {
		inertMatch(power());
	}
	return validMatch = true;
}

bool CompileUnit::power() {
	inertMatch(atom());
	while (true) {
		if (matchCTK(TokenKind::pow)) {
			striveMatch(factor(), CTKNEER, RepId::expectedExpr, tokenKindName(CTK));
			vm->write(Bytecode::pow);
		} else { break; }
	}
	return validMatch = true;
}

bool CompileUnit::atom() {
	if (matchCTK(TokenKind::num)) {
		vm->write(Bytecode::ldc, vm->constList.reg(PT.value), -1);
	} else if (matchCTK(TokenKind::str)) {
		vm->write(Bytecode::ldc, vm->constList.reg(PT.value, &cmpFstrOfValue), -1);
	} else if (matchCTK(TokenKind::lpare)) {
		striveMatch(expr(), CTK != TokenKind::rpare, RepId::expectedExpr, tokenKindName(CTK));
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

