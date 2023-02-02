#include "ilexer.h"
#include <cstdio>
#include <cctype>
#include "../report/ireport.h"

//ANT -> Token to be Analyzed
#define ANT thiToken

//macro for advance()
#define initANTData()                       \
skipBlanks();                               \
ANT.extract.data = nextCharPtr - 1;         \
ANT.value.i64 = 0;                          \
ANT.pos = pos;


Lexer::Lexer(const char *file) {
	init(file);
}

Lexer::~Lexer() {
	if (code == nullptr) { return; }
	delete[] code;
}


void Lexer::init(const char *file) {
	assert(~fileName == nullptr, "Lexer cannot be initialized twice!");
	fileName = file;
	readFile();
	init();
}

void Lexer::advance() {
	//推进Token
	preToken = curToken;
	curToken = nexToken;
	nexToken = secToken;
	secToken = ANT;

//	skipBlanks();                   //跳过空白符,此句已被下一句包含
	initANTData();                  //设置ANT初始状态
	ANT.kind = TokenKind::eof;      //设置类型初始值

	while (curChar != '\0') {       //循环判断
		switch (curChar) {
			case '+':
				ANT.kind = TokenKind::add;
				break;
			case '-':
				ANT.kind = TokenKind::sub;
				break;
			case '*':
				ANT.kind = matchNextChar('*') ?
				           TokenKind::pow : TokenKind::mul;
				break;
			case '/':
				if (matchNextChar('/') || matchNextChar('*')) {
					skipComment(curChar == '*');
					initANTData();//重走一遍初始化流程
					continue;
				}
				ANT.kind = TokenKind::div;
				break;
			case '%':
				ANT.kind = TokenKind::mod;
				break;
			case '(':
				ANT.kind = TokenKind::lpare;
				++unLpare;
				break;
			case ')':
				ANT.kind = TokenKind::rpare;
				--unLpare;
				break;
			case '[':
				ANT.kind = TokenKind::lbracket;
				++unLbracket;
				break;
			case ']':
				ANT.kind = TokenKind::rbracket;
				--unLbracket;
				break;
			case '{':
				ANT.kind = TokenKind::lbrace;
				++unLbrace;
				break;
			case '}':
				ANT.kind = TokenKind::rbrace;
				--unLbrace;
				break;
			case ';':
				ANT.kind = TokenKind::end;
				break;
			case '"':
				ANT.kind = TokenKind::str;
				parseString();
				return;
			default:
				if (isalpha(curChar) || curChar == '_') {
					parseId(TokenKind::unk);
				} else if (isdigit(curChar)) {
					parseNum();
				} else {
					reportMsg(RepId::unsupportedChar, this, curChar);
					getNextChar();//pass掉当前这个unsupportedChar
					initANTData();//重走一遍初始化流程
					continue;
				}
				return;
		}
		ANT.extract.length = nextCharPtr - ~ANT.extract;
		getNextChar();
		return;
	}
}

void Lexer::init() {
	nextCharPtr = (char *) (code + 1);
	curChar = code[0];
	advance();//推进到thiToken
	advance();//推进到secToken
	advance();//推进到nexToken
	advance();//推进到curToken
}

void Lexer::readFile() {
	if (~fileName == nullptr) { reportMsg(RepId::fileNameNullptr, nullptr); }
	FILE *fp = fopen(~fileName, "r");
	if (fp == nullptr) { reportMsg(RepId::failToReadFile, nullptr, ~fileName); }
	fseek(fp, 0L, SEEK_END);
	uint64_t fileSize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	char *_code = (char *) calloc(fileSize + 1, 1);
	fread(_code, 1, fileSize, fp);
	_code[fileSize] = '\0';
	fclose(fp);
	code = _code;
}

void Lexer::skipBlanks() {
	while (isspace(curChar)) { getNextChar(); }
}

void Lexer::skipComment(bool isBlock) {
	while (curChar != '\0') {
		if (isBlock) {
			if (curChar == '*' && matchNextChar('/')) {
				getNextChar();
				return;
			}
			if (*nextCharPtr == '\0') {//到结尾还是没有块注释结束符,便整个警告
				getNextChar();
				reportMsg(RepId::unterminatedBlockComment, this);
			}
		} else {
			if (curChar == '\n') {
				getNextChar();
				return;
			}
		}
		getNextChar();
	}
}

void Lexer::parseNum() {
	uint8_t dotNum = 0;
	while (isdigit(curChar) || curChar == '.') {
		if (curChar == '.') { dotNum++; }
		getNextChar();
	}
	if (dotNum > 1) { reportMsg(RepId::invalidNumberFormat, this); }
	isize length = nextCharPtr - ~ANT.extract - 1;
	ANT.kind = TokenKind::num;
	ANT.extract.length = length;
	ANT.value.f64 = strtod(~ANT.extract, nullptr);
}

void Lexer::parseString() {
	getNextChar();
	while (curChar != '\0' && curChar != '\n' && curChar != '"') { getNextChar(); }
	if (curChar != '"') { reportMsg(RepId::unterminatedString, this); }
	isize length = nextCharPtr - ~ANT.extract;
	getNextChar();//此时的curChar是'"'
	ANT.extract.length = length;
	ANT.value.fsp = new refString(~ANT.extract + 1, length - 2);
}

void Lexer::parseId(TokenKind kind) {
	while (isalnum(curChar) || curChar == '_') { getNextChar(); }
	isize length = nextCharPtr - ~ANT.extract - 1;
	ANT.extract.length = length;
	ANT.kind = (kind != TokenKind::unk ? kind : idOrKeyword(ANT.extract));
}

TokenKind Lexer::idOrKeyword(refString &str) {
//TODO idOrKeyword

//	TokenKind kind = isKeyword(str);
//	if (kind == TokenKind::unk) { kind = TokenKind::id; }
	return TokenKind::id;
}

void Lexer::getNextChar() {
	if (curChar != '\0') {
		++pos.column;            //推进列号
		if (curChar == '\n') {   //遇到了换行符
			++pos.line;          //推进行号
			pos.column = 1;      //重置列号
		}
		curChar = *(nextCharPtr++); //推进字符
	}
}

bool Lexer::matchNextChar(char value) {
	if (*nextCharPtr == value) {
		//仅推进一次是因为,advance的switch语句里也会推进
		getNextChar();
		return true;
	}
	return false;
}
