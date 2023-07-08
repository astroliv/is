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

//惰性匹配,匹配失败就摆烂返回false(其实就是CaseFalseReturnFalse)
#define inertMatch(fn) \
if((fn())!=Status::success){return Status::failure;}

//强制匹配,匹配失败就开摆,报错返回一气呵成
#define forceMatch(fn, id, ...) \
switch (fn()) {\
    case Status::failure:reportMsg(id,this,##__VA_ARGS__);\
    case Status::error:return Status::error;\
    case Status::success:break;\
}

#define assertCTK(kind) \
if(CTK!=(kind)){\
    if(doGenerate){\
        reportMsg(RepId::assertTokenKindFailure,this,\
        tokenKindInfo(kind).strName,tokenKindInfo(CTK).strName);\
        return Status::error;\
    }else{return Status::failure;}\
}advance();


bool cmpFstrOfValue(const Value &a, const Value &b);//比较俩Value的fstr数据是否相等

CompileUnit::CompileUnit(const char *file, Compiler *_com) {
	init(file, _com);
}

inline void CompileUnit::init(const char *file, Compiler *_com) {
	lexer.init(file);
	com = _com;
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

inline TokenKind CompileUnit::selectMatchCTK(TokenKind tk1, TokenKind tk2) {
	if (CTK != tk1 && CTK != tk2) { return TokenKind::unk; }
	return advance().kind; //Pass curToken
}

inline TokenKind CompileUnit::selectMatchCTK(TokenKind tk1, TokenKind tk2, TokenKind tk3) {
	if (CTK != tk1 && CTK != tk2 && CTK != tk3) { return TokenKind::unk; }
	return advance().kind; //Pass curToken
}

//inline void CompileUnit::assertCTK(TokenKind kind) {
//	if (CTK != kind) {
//		reportMsg(RepId::assertTokenKindFailure, this,
//		          tokenKindInfo(kind).strName,
//		          tokenKindInfo(CTK).strName);
//		errCurFile = errCurStmt = true;
//		return;//由于当前token不是所预期的,那么就认为其为下一个匹配所需的,不应被pass掉
//	}
//	advance();//Pass curToken
//}

inline void CompileUnit::skipCurStmt() {
	while (CTK != TokenKind::end && CTK != TokenKind::eof) { advance(); }
}


Status CompileUnit::stmts() {
	while (CTK != TokenKind::eof) {
		switch (expr()) {
			case Status::error://报错时reportMsg会自动设置errCurFile为true
			case Status::failure:
				skipCurStmt();
//				reportMsg(RepId::unknownStatement, this);
			case Status::success:
			assertCTK(TokenKind::end);
		}
//		if (lexer.unLpare || lexer.unLbracket || lexer.unLbrace) {
//			reportMsg(RepId::unmatchedBracketsStmt, this);
//			lexer.unLpare = lexer.unLbracket = lexer.unLbrace = 0;
//		}//这只是一个提示,因为实际上assertTokenKindFailure也会对该情况进行报错
	}
	com->write(Instruction::end);
	return Status::success;

}

Status CompileUnit::expr() {
	inertMatch(term);
	while (true) {
		TokenKind tk = selectMatchCTK(TokenKind::add, TokenKind::sub);
		if (tk == TokenKind::unk) { break; }
		if (doGenerate) {
			forceMatch(term, RepId::expectedExpr, tokenKindInfo(CTK).strName);
			com->write(relatedInstruction(tk));
		} else inertMatch(term);
	}
	return Status::success;
}

Status CompileUnit::term() {
	inertMatch(factor);
	while (true) {
		TokenKind tk = selectMatchCTK(TokenKind::mul, TokenKind::div, TokenKind::mod);
		if (tk == TokenKind::unk) { break; }
		if (doGenerate) {
			forceMatch(factor, RepId::expectedExpr, tokenKindInfo(CTK).strName);
			com->write(relatedInstruction(tk));
		} else inertMatch(factor);
	}
	return Status::success;
}

Status CompileUnit::factor() {
	TokenKind tk = selectMatchCTK(TokenKind::add, TokenKind::sub);
	if (tk == TokenKind::unk) {
		inertMatch(power);
		return Status::success;
	}
	if (doGenerate) {
		forceMatch(factor, RepId::expectedExpr, tokenKindInfo(CTK).strName);
		if (tk == TokenKind::sub) { com->write(Instruction::neg); }
	} else inertMatch(factor);
	return Status::success;
}

Status CompileUnit::power() {
	inertMatch(atom);
	while (true) {
		if (!matchCTK(TokenKind::pow)) { break; }
		if (doGenerate) {
			forceMatch(factor, RepId::expectedExpr, tokenKindInfo(CTK).strName);
			com->write(Instruction::pow);
		} else inertMatch(factor);
	}
	return Status::success;
}

Status CompileUnit::atom() {
	TokenKind tk = selectMatchCTK(TokenKind::num, TokenKind::str, TokenKind::lpare);
	if (tk == TokenKind::unk) { return Status::failure; }
	if (doGenerate) {
		if (tk == TokenKind::num) {
			com->write(Instruction::ldc, com->vm->constList.reg(PT.value), -1);
		} else if (tk == TokenKind::str) {
			com->write(Instruction::ldc, com->vm->constList.reg(PT.value, &cmpFstrOfValue), -1);
		} else if (tk == TokenKind::lpare) {
			forceMatch(expr, RepId::expectedExpr, tokenKindInfo(CTK).strName);
			assertCTK(TokenKind::rpare);
		}
	} else if (tk == TokenKind::lpare) {
		inertMatch(expr);
		assertCTK(TokenKind::rpare);
	}
	return Status::success;
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


bool cmpFstrOfValue(const Value &a, const Value &b) {
	//由于Array<T>内查找与注册等方法的比较都是直接作==运算,那么对于Value类型,就要定义一个==的操作符重载,才能
	//使用Array<Value>.为了普适性,我为Value定义的是直接进行内容比较.这样的话要是比较字符串,那就相当于直接比较
	//指针,这肯定不是我们想要的结果.于是我为Array<T>设计了一个比较的方式,可以指定用于比较的函数,然后使用该函数
	//比较,由此进行查找,注册等方法.于是在我们要比较字符串内容时,只需要指定这个函数,便可以解决其只会比较指针的问题
	return *a.fsp == *b.fsp;
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

void Compiler::write(Instruction type, int64_t operand, int8_t len) {
	write(type);
	write(operand, len);
}

inline void Compiler::writeVarg(uint64_t arg) {
	uint8_t i = 0;
	do {
		instream.append((arg >> 7) && (i < 8) ? arg & 0x7F | 0x80 : arg);
	} while ((arg >>= 7) && (i++ < 8));
}


