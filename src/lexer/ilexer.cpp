#include "ilexer.h"
#include <cstdio>
#include <cctype>

//ANT -> Token to be Analyzed
#define ANT thiToken

#define initANTData()                       \
ANT.extract.setData(nextCharPtr - 1);       \
ANT.value.i64 = 0;                          \
ANT.pos = pos;


Lexer::Lexer(const char *file) {
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

	skipBlanks();                   //跳过空白符
	initANTData();               //设置ANT初始状态
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
					skipCommit(curChar == '*');
					skipBlanks();
					initANTData();
					continue;
				}
				ANT.kind = TokenKind::div;
				break;
			case '%':
				ANT.kind = TokenKind::mod;
				break;
			case '(':
				ANT.kind = TokenKind::lpare;
				break;
			case ')':
				ANT.kind = TokenKind::rpare;
				break;
			case '[':
				ANT.kind = TokenKind::lbracket;
				break;
			case ']':
				ANT.kind = TokenKind::rbracket;
				break;
			case '{':
				ANT.kind = TokenKind::lbrace;
				break;
			case '}':
				ANT.kind = TokenKind::rbrace;
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
					assert(false, "Unsupported char '%c'", curChar);
				}
				return;
		}
		ANT.extract.setLength(nextCharPtr - ~ANT.extract);
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
	assert(~fileName != nullptr, "Unreachable Branch!");//TODO error
	FILE *fp = fopen(~fileName, "r");
	assert(fp != nullptr, "Read file failed!");//TODO error
	fseek(fp, 0L, SEEK_END);
	uint32_t fileSize = ftell(fp);
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

void Lexer::skipCommit(bool isBlock) {
	while (curChar != '\0') {
		if (isBlock) {
			if (curChar == '*' && matchNextChar('/')) {
				getNextChar();
				return;
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
	assert(dotNum <= 1, "Invalid number format.")//TODO error
	isize length = nextCharPtr - ~ANT.extract - 1;
	ANT.kind = TokenKind::num;
	ANT.extract.setLength(length);
	ANT.value.f64 = strtod(~ANT.extract, nullptr);
}

void Lexer::parseString() {
	getNextChar();
	while (curChar != '\0' && curChar != '\n' && curChar != '"') { getNextChar(); }
	assert(curChar == '"', "unterminated string.");//TODO error
	isize length = nextCharPtr - ~ANT.extract;
	getNextChar();//此时的curChar是'"'
	ANT.extract.setLength(length);
	ANT.value.fstr = new refString(~ANT.extract + 1, length - 2);
}

void Lexer::parseId(TokenKind kind) {
	while (isalnum(curChar) || curChar == '_') { getNextChar(); }
	isize length = nextCharPtr - ~ANT.extract - 1;
	ANT.extract.setLength(length);
	ANT.kind = (kind != TokenKind::unk ? kind : idOrKeyword(ANT.extract));
}

TokenKind Lexer::idOrKeyword(refString &str) {
//TODO
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

