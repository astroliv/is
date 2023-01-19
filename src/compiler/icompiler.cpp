#include "icompiler.h"
#include "../utils/ireport.h"

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

//case false return false
#define CFRF(sign) \
if(!(sign)){return false;}

bool cmpFstrOfValue(Value a, Value b);//比较俩Value的fstr数据是否相等

CompileUnit::CompileUnit(const char *file, VM *_vm) : vm(_vm) {
	lexer.init(file);
	module = new Module();//以后会换成~compiler.moduleList
	module->file = file;
	instream.byteStream = new ByteStream(16);

}

void CompileUnit::compile() {
	if (!stmts()) {
		unreachableBranch();
	}
}

inline Token CompileUnit::advance() {
	lexer.advance();
	return PT;
}

inline bool CompileUnit::matchNTK(TokenKind kind) {
	if (NTK == kind) {
		advance();  //passCT
		advance();  //passNT
		return true;
	}
	return false;
}

inline bool CompileUnit::matchCTK(TokenKind kind) {
	if (CTK != kind) { return false; }
	advance();
	return true;
}


inline void CompileUnit::assertCTK(TokenKind kind) {
	if (CTK != kind) {
		reportMsg(RepId::assertTokenKindFailure, this,
		          tokenKindName[(uint16_t) kind], tokenKindName[(uint16_t) NTK]);
	}
	advance();  //passCT
}


bool CompileUnit::stmts() {
	while (CTK != TokenKind::eof) {
		if (!expr()) {
			unreachableBranch();
			return false;
		}
		assertCTK(TokenKind::end);
	}
	instream.write(Bytecode::end);
	return true;
}

bool CompileUnit::expr() {
	CFRF(term());//匹配失败则返回false
	while (true) {
		if (matchCTK(TokenKind::add)) {//编译加法
			CFRF(term());//匹配失败则返回false
			instream.write(Bytecode::add);
		} else if (matchCTK(TokenKind::sub)) {//编译减法
			CFRF(term());//匹配失败则返回false
			instream.write(Bytecode::sub);
		} else { break; }
	}
	return true;
}

bool CompileUnit::term() {
	CFRF(factor());//匹配失败则返回false
	while (true) {
		if (matchCTK(TokenKind::mul)) {//编译乘法
			CFRF(factor());//匹配失败则返回false
			instream.write(Bytecode::mul);
		} else if (matchCTK(TokenKind::div)) {//编译除法
			CFRF(factor());//匹配失败则返回false
			instream.write(Bytecode::div);
		} else if (matchCTK(TokenKind::mod)) {//编译取余
			CFRF(factor());//匹配失败则返回false
			instream.write(Bytecode::mod);
		} else { break; }
	}
	return true;
}

bool CompileUnit::factor() {
	if (matchCTK(TokenKind::add)) {//取本身
		CFRF(factor());
	} else if (matchCTK(TokenKind::sub)) {
		CFRF(factor());
		instream.write(Bytecode::neg);//取相反数
	} else { CFRF(power()); }
	return true;
}

bool CompileUnit::power() {
	CFRF(atom());
	while (true) {
		if (matchCTK(TokenKind::pow)) {//编译幂运算
			CFRF(factor());
			instream.write(Bytecode::pow);
		} else { break; }
	}
	return true;
}

bool CompileUnit::atom() {
	if (matchCTK(TokenKind::num)) {//编译数值字面量
		instream.write(Bytecode::ldc, 0, 4);
		instream.write(module->constList.reg(PT.value), 4);
		return true;
	} else if (matchCTK(TokenKind::str)) {//编译字符串字面量
		instream.write(Bytecode::ldc, 0, 4);
		instream.write(module->constList.reg(PT.value, &cmpFstrOfValue), 4);
	} else if (matchCTK(TokenKind::lpare)) {//括号包含的表达式
		CFRF(expr());
		assertCTK(TokenKind::rpare);
	} else {
		return false;
	}
	return true;
}

bool cmpFstrOfValue(Value a, Value b) {
	//由于Array<T>内查找与注册等方法的比较都是直接作==运算,那么对于Value类型,就要定义一个==的操作符重载,才能
	//使用Array<Value>.为了普适性,我为Value定义的是直接进行内容比较.这样的话要是比较字符串,那就相当于直接比较
	//指针,这肯定不是我们想要的结果.于是我为Array<T>设计了一个比较的方式,可以指定用于比较的函数,然后使用该函数
	//比较,由此进行查找,注册等方法.于是在我们要比较字符串内容时,只需要指定这个函数,便可以解决其只会比较指针的问题
	return *a.fstr == *b.fstr;
}

