#include "ilexer.h"
#include <cstdio>
#include <cctype>

//ANT -> Token to be Analyzed
#define ANT thiToken

#define setANTPosition()                    \
ANT.extract.setData(nextCharPtr - 1);       \
ANT.pos = pos;


Lexer::Lexer(const char *file) {
	fileName = file;
	readFile();
	init();
}

void Lexer::init() {
	nextCharPtr = (char *) (code + 1);
	curChar = code[0];
	advance();//推进到thiToken
	advance();//推进到secToken
	advance();//推进到nexToken
	advance();//推进到curToken
}

void Lexer::advance() {
	//推进Token
	preToken = curToken;
	curToken = nexToken;
	nexToken = secToken;
	secToken = ANT;

	skipBlanks();                   //跳过空白符
	setANTPosition();               //设置ANT初始状态
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
					setANTPosition();
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
				break;
			default:
				//TODO parseId()
				//TODO parseNum()
				break;
		}
		ANT.extract.setLength(nextCharPtr - ~ANT.extract);
		getNextChar();
		return;
	}


}

void Lexer::readFile() {
	assert(fileName.getData() != nullptr, "Unreachable Branch!");
	FILE *fp = fopen(fileName.getData(), "r");
	assert(fp != nullptr, "Read file failed!");
	fseek(fp, 0L, SEEK_END);
	uint32_t fileSize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	char *_code = (char *) calloc(fileSize + 1, 1); // 这里最好用calloc不然会出bug(new出来的未清零)
	fread(_code, 1, fileSize, fp);
	_code[fileSize] = '\0';
	fclose(fp);
	code = _code;
}

void Lexer::skipBlanks() {
	while (isspace(curChar)) { getNextChar(); }
}

void Lexer::getNextChar() {
	if (curChar != '\0') {
		pos.column++;        //推进列号
		if (curChar == '\n') //遇到了换行符
		{
			pos.line++;     //推进行号
			pos.column = 1; //重置列号
		}
		curChar = *nextCharPtr++; //推进字符
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

void Lexer::parseString() {
	getNextChar();
	while (curChar != '\0' && curChar != '\n' && curChar != '"') {
		getNextChar();
	}
	assert(curChar == '"', "unterminated string.");//TODO error
	getNextChar();
	isize length = nextCharPtr - ~ANT.extract - 1;
	ANT.extract.setLength(length);
	ANT.value.fstr = new refString(~ANT.extract + 1, length - 2);
}

void Lexer::parseNum() {
	uint8_t dotNum = 0;
	while (isdigit(curChar) || curChar == '.') {
		if (curChar == '.') { dotNum++; }
		getNextChar();
	}
	assert(dotNum == 1, "Invalid number format.")//TODO error
	isize length = nextCharPtr - ~ANT.extract - 1;
	ANT.kind = TokenKind::num;
	ANT.extract.setLength(length);
	ANT.value.f64 = strtod(~ANT.extract, nullptr);
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


