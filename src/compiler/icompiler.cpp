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

//下面是几个宏,用于处理匹配报错的

//惰性匹配,匹配失败就摆烂返回fn的返回值
#define inertMatch(fn) \
do{\
    switch(fn()){\
        case Status::failure:return Status::failure;\
        case Status::error:return Status::error;\
        case Status::success:break;\
    }\
}while(0);

//强制匹配,匹配失败就报错返回Status::error
#define forceMatch(fn, id, ...) \
do{\
    switch(fn()){\
        case Status::failure:reportMsg(id,this,##__VA_ARGS__);\
        case Status::error:return Status::error;\
        case Status::success:break;\
    }\
}while(0);

//#define aheadMatch(fn) \
//do{\
//    assert(doGenerate, "doGenerate is false here.");\
//    lexer.startLookahead();\
//    doGenerate = false;\
//    result = fn();\
//    doGenerate = true;\
//    lexer.endLookahead();\
//}while(0);

//断言当前token类型,失败则报错返回Status::error
#define assertCTK(kind) \
do{\
    if(CTK!=(kind)){\
        if(doGenerate){\
            reportMsg(RepId::assertTokenKindFailure,this,\
            tokenKindInfo(kind).strName,tokenKindInfo(CTK).strName);\
            return Status::error;\
        }else{return Status::failure;}\
    }advance();\
}while(0);

//compare methods
namespace cmp {
	//此函数功能为比较俩Value的fsp指向的refString数据是否相等,实际上也可以比较string数据
	//由于Array<T>内查找与注册等方法的比较都是直接作==运算,那么对于Value类型,就要定义一个==的操作符重载,才能
	//使用Array<Value>.为了普适性,我为Value定义的是直接进行内容比较.这样的话要是比较字符串,那就相当于直接比较
	//指针,这肯定不是我们想要的结果.于是我为Array<T>设计了一个比较的方式,可以指定用于比较的函数,然后使用该函数
	//比较,由此进行查找,注册等方法.于是在我们要比较字符串内容时,只需要指定这个函数,便可以解决其只会比较指针的问题
	//此函数选用下划线的命名方式,因为有缩写不适合驼峰的那种(太丑了)
	bool value_fsp(const Value &a, const Value &b) {
		return *a.fsp == *b.fsp;
	}
}

CompileUnit::CompileUnit(const char *file, Compiler *_com) {
	init(file, _com);
}

inline void CompileUnit::init(const char *file, Compiler *_com) {
	lexer.init(file);
	com = _com;
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

inline TokenKind CompileUnit::selectMatchCTK(TokenKind tk1, TokenKind tk2) {
	if (CTK != tk1 && CTK != tk2) { return TokenKind::unk; }
	return advance().kind; //Pass curToken
}

inline TokenKind CompileUnit::selectMatchCTK(TokenKind tk1, TokenKind tk2, TokenKind tk3) {
	if (CTK != tk1 && CTK != tk2 && CTK != tk3) { return TokenKind::unk; }
	return advance().kind; //Pass curToken
}

inline void CompileUnit::skipCurStmt() {
	while (CTK != TokenKind::end && CTK != TokenKind::eof) { advance(); }
}


void CompileUnit::compile() {
	printf("%d\n", lookahead(&CompileUnit::stmts));
//	if (!lookahead(&CompileUnit::stmts)) { return; }
	stmts();
	if (doGenerate) { com->write(Instruction::end); }
}

//stmts -> (<end>|expr <end>|def_var)*
Status CompileUnit::stmts() {
	while (CTK != TokenKind::eof) {
		if (matchCTK(TokenKind::end)) { continue; }
		switch (expr()) {//目前就只有一个expr,还没搞def_var
			case Status::failure://匹配失败则表明该表达式为未知类型
				reportMsg(RepId::unknownStatement, this);
			case Status::error://报错时reportMsg会自动设置errCurFile为true
				skipCurStmt();//跳过当前语句即可
			case Status::success:
				assertCTK(TokenKind::end);//成功则断言一下结束符
		}
	}
	return Status::success;//匹配完则返回success
}

//expr -> term ([<add>|<sub>] term)*
Status CompileUnit::expr() {
	inertMatch(term);
	while (true) {
		auto tkind = selectMatchCTK(TokenKind::add,
		                            TokenKind::sub);
		//不为add和sub则表明不应再匹配后面的语句,跳出循环
		if (tkind == TokenKind::unk) { break; }
		//需要生成指令时,该匹配必须为term,不为则是语法错误,所以用forceMatch,匹配失败便报错返回
		if (doGenerate) {
			forceMatch(term, RepId::expectedExpr, tokenKindStrName(CTK));
			com->write(relatedInstruction(tkind));//写指令
		} else { inertMatch(term); }//不生成时,直接测试匹配即可(不生成时不应该报错)
	}
	return Status::success;
}

//term -> factor ([<mul>|<div>|<mod>] factor)*
Status CompileUnit::term() {
	inertMatch(factor);
	while (true) {
		auto tkind = selectMatchCTK(TokenKind::mul,
		                            TokenKind::div,
		                            TokenKind::mod);
		//不为mul和div和mod则表明不应再匹配后面的语句,跳出循环
		if (tkind == TokenKind::unk) { break; }
		if (doGenerate) {
			//需要生成指令时,该匹配必须为factor,不为则是语法错误,所以用forceMatch,匹配失败便报错返回
			forceMatch(factor, RepId::expectedExpr, tokenKindStrName(CTK));
			com->write(relatedInstruction(tkind));//写指令
		} else { inertMatch(factor); }//不生成时,直接测试匹配即可
	}
	return Status::success;
}

//factor -> [<add>|<sub>] factor | power
Status CompileUnit::factor() {
	auto tkind = selectMatchCTK(TokenKind::add,
	                            TokenKind::sub);
	if (tkind == TokenKind::unk) {
		//不为add和sub则表明应为另一匹配式,即power
		inertMatch(power);
	} else if (doGenerate) {
		//需要生成指令时,该匹配必须为factor,不为则是语法错误,所以用forceMatch,匹配失败便报错返回
		forceMatch(factor, RepId::expectedExpr, tokenKindStrName(CTK));
		//加号无需操作,减号则写指令neg
		if (tkind == TokenKind::sub) { com->write(Instruction::neg); }
	} else { inertMatch(factor); }//不生成时,直接测试匹配即可
	return Status::success;
}

//power -> atom (<pow> factor)*
Status CompileUnit::power() {
	inertMatch(atom);
	while (true) {
		//不为pow则表明不应再匹配后面的语句,跳出循环
		if (!matchCTK(TokenKind::pow)) { break; }
		if (doGenerate) {
			//需要生成指令时,该匹配必须为factor,不为则是语法错误,所以用forceMatch,匹配失败便报错返回
			forceMatch(factor, RepId::expectedExpr, tokenKindStrName(CTK));
			com->write(Instruction::pow);//写指令
		} else { inertMatch(factor); }//不生成时,直接测试匹配即可
	}
	return Status::success;
}

//atom -> <real> | <str> | <id> | <lpare> expr <rpare>
Status CompileUnit::atom() {
	auto tkind = selectMatchCTK(TokenKind::real,
	                            TokenKind::str,
	                            TokenKind::lpare);
	if (doGenerate) {//生成指令
		if (tkind == TokenKind::unk) {
			//生成指令,且当前匹配未知,是语法错误
			reportMsg(RepId::requireValue, this);
			return Status::error;
		} else if (tkind == TokenKind::real) {//匹配为实数字面量
			com->write(Instruction::ldc,
			           com->vm->constList.reg(PT.value));
		} else if (tkind == TokenKind::str) {//匹配为字符串字面量
			com->write(Instruction::ldc,
			           com->vm->constList.reg(PT.value, cmp::value_fsp));
		} else if (tkind == TokenKind::lpare) {//匹配为括号表达式
			forceMatch(expr, RepId::expectedExpr, tokenKindStrName(CTK));
			assertCTK(TokenKind::rpare);//表达式结束必须要有右括号匹配的~
		}
	} else if (tkind == TokenKind::unk) {
		//不生成指令,且当前匹配未知,因为是测试性匹配,返回failure
		return Status::failure;
	} else if (tkind == TokenKind::lpare) {
		//不生成指令,匹配为括号表达式
		inertMatch(expr);
		assertCTK(TokenKind::rpare);//assertCTK自带对doGenerate的判断的
	}
	return Status::success;
}


bool CompileUnit::lookahead(MatchFn fn) {
	//有可能是fn的某子项调用,不过保险起见还是加上了这一句
	assert(doGenerate, "doGenerate is false here.");
	//下面是核心语句
	doGenerate = false;
	lexer.startLookahead();
	Status status = (this->*fn)();//用当前cu实例调用fn
	lexer.endLookahead();
	doGenerate = true;
	//测试性匹配不应报错,而error是报完错才会返回来的
	assert(status != Status::error, "lookahead got Status::error.");
	return status == Status::success;
}

inline Instruction CompileUnit::relatedInstruction(TokenKind tk) {
	switch (tk) {
		case TokenKind::add:
			return Instruction::add;
		case TokenKind::sub:
			return Instruction::sub;
		case TokenKind::mul:
			return Instruction::mul;
		case TokenKind::div:
			return Instruction::div;
		case TokenKind::pow:
			return Instruction::pow;
		case TokenKind::mod:
			return Instruction::mod;
		default:
			unreachableBranch();
	}
	return Instruction::unk;
}

Compiler::Compiler(VM *_vm) {
	init(_vm);
}

Compiler::Compiler(const char *file, VM *_vm) {
	init(file, _vm);
}

void Compiler::init(VM *_vm) {
	cuList.append().com = this;
	vm = _vm;
}

void Compiler::init(const char *file, VM *_vm) {
	mkCompileUnit(file);
	vm = _vm;
}

inline void Compiler::mkCompileUnit(const char *file) {
	cuList.append().init(file, this);
}

void Compiler::write(Instruction type) {
	instream.append((byte) type);
}

void Compiler::write(int64_t operand, int8_t len) {
	switch (len) {//直接穷举,没必要整循环
		case -1:
			writeVarg(operand);
			return;
		case 1:
			instream.append(operand);
			return;
		case 2:
			instream.append(operand);
			instream.append(operand >> 8);
			return;
		case 4:
			instream.append(operand);
			instream.append(operand >> 8);
			instream.append(operand >> 16);
			instream.append(operand >> 24);
			return;
		case 8:
			instream.append(operand);
			instream.append(operand >> 8);
			instream.append(operand >> 16);
			instream.append(operand >> 24);
			instream.append(operand >> 32);
			instream.append(operand >> 40);
			instream.append(operand >> 48);
			instream.append(operand >> 56);
			return;
		default:
			unreachableBranch();
	}
}

void Compiler::write(Instruction type, int64_t operand) {
	write(type);
	write(operand, instructionInfo(type).fopLen[0]);
}

void Compiler::write(Instruction type, int64_t operand1, int64_t operand2) {
	write(type);
	write(operand1, instructionInfo(type).fopLen[0]);
	write(operand2, instructionInfo(type).fopLen[1]);
}

inline void Compiler::writeVarg(uint64_t arg) {
	uint8_t i = 0;
	do {
		instream.append((arg >> 7) && (i < 8) ? arg & 0x7F | 0x80 : arg);
	} while ((arg >>= 7) && (i++ < 8));
}


