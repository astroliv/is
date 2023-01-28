#ifndef IS_ILEXER_H
#define IS_ILEXER_H
#include "itoken.h"

class Lexer {
public:
	string fileName;             //该Lexer解析的文件
	const char *code{nullptr};   //源码指针
	char *nextCharPtr{nullptr};  //下一个字符的指针
	char curChar{0};             //当前字符
	Position pos;                //当前解析到的位置

	//解析Token列表

	Token preToken;              //上一个token
	Token curToken;              //这一个token
	Token nexToken;              //下一个token
	Token secToken;              //下二个token
	Token thiToken;              //下三个token


	Lexer() = default;                   //无参构造
	explicit Lexer(const char *file);    //文件构造

	~Lexer();                            //析构

	void init(const char *file);         //初始化

	void advance(); //推进Token,即获取下一个Token

private:
	void init();            //初始化
	void readFile();        //读指定文件

	void skipBlanks();              //跳过空白字符
	void skipCommit(bool isBlock);  //跳过注释

	void parseNum();                //解析数字
	void parseString();             //解析字符串
	void parseId(TokenKind kind);   //解析标识符

	TokenKind idOrKeyword(refString &str); //判断是否为关键词

	void getNextChar();               //推进到下一个字符
	bool matchNextChar(char value);   //匹配下一个字符,成功返回true并推进一次字符
};


#endif //IS_ILEXER_H
