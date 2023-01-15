#include <cstdio>
#include "utils/iarray.h"
#include "utils/ibytecode.h"
#include "utils/istring.h"
#include "lexer/ilexer.h"

void testArray();
void testString();
void testFree();
void testLexer();

class TestFree {
public:
	TestFree() { printf("TestFree:Test create.\n"); };

	~TestFree() { printf("~TestFree:Test free!\n"); }
};

TestFree retTestFree() {
	TestFree tmp;
	return tmp;
}

void testFree() {
	printf("fn:testFree->begin.\n");
	for (isize i = 0; i < 8; ++i) {
		printf("for->begin.\n");
		retTestFree();
		printf("for->end.\n");
	}
	printf("fn:testFree->end.\n");
}

int main() {
//    testArray();
//	testString();
	testLexer();
//	testFree();
	return 0;
}

void testLexer() {
	Lexer lex(R"(E:\projects\is\script\test.is)");
	while (lex.preToken.kind != TokenKind::eof) {
		printf("%s\n", ~lex.curToken.dump());
		lex.advance();
	}
}


void testString() {

	const char *strInit = "123";
	string a(strInit);
	string b(strInit);
	printf("a:%p\tb:%p\tinit:%p\n", ~a, ~b, strInit);


//    refString s("refString");
//    printf("%u\n", s.getLength());
//    string s1("string1");
//    string s2("string2");
//    string s3("string3");
//    string str(s1);
//    printf("str:%s\ts1:%s\ts2:%s\ts3:%s\n", ~str, ~s1, ~s2, ~s3);
//    str[2] = 'a';
//    printf("str:%s\ts1:%s\ts2:%s\ts3:%s\n", ~str, ~s1, ~s2, ~s3);
//    str = s3;
//    str[4] = '5';
//    char *cstr = ~str;
//    printf("str:%s\ts1:%s\ts2:%s\ts3:%s\n", ~str, ~s1, ~s2, ~s3);
//    str[12] = '1';
//    str = "123";
//    printf("str:%s\ts1:%s\ts2:%s\ts3:%s\n", ~str, ~s1, ~s2, ~s3);
//    printf("cstr:%s\ts1:%s\ts2:%s\ts3:%s\n", cstr, ~s1, ~s2, ~s3);
}

void testArray() {
	Array<uint8_t> vary(4);
	vary.add(100);
	vary.add(20);
	vary.add(3);
	vary.add(4);
	vary.add(4);
	vary.append(5);
	vary[3] = 12;
	vary[4] = 5;
	int32_t ary[4] = {1, 2, 3, 4};
	writeArg(vary, -1);
	writeArg(vary, -1);
	writeArg(vary, -1);
	writeArg(vary, -1);
	writeArg(vary, -1);
	printf("%llu\n", readArg(vary, 5));
	for (isize i = 0; i < vary.getUsedSize(); ++i) {
		printf("vary [%d]:%d\n", i, vary[i]);
	}
	printf("varyCapacity:%d/%d\n", vary.getUsedSize(), vary.getCapacity());
	vary[666] = 0;
	printf("A statement after index out of range.\n");
	vary[666] = 0;
}


