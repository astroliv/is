#include <cstdio>
#include <cstring>
#include <windows.h>
#include "container/iarray.h"
#include "container/istack.h"
#include "basic/ibytecode.h"
#include "string/istring.h"
#include "report/ireport.h"
#include "lexer/ilexer.h"
#include "compiler/icompiler.h"
#include "vm/ivm.h"
#include "container/imodifier.h"

void testArray();
void testString();
void testCompound();
void testLexer();
void testCompiler();

int main() {
//	setbuf(stdout, nullptr);
//	testArray();
//	testString();
//	testLexer();
	testCompiler();
//	testCompound();
	return 0;
}

void testCompound() {
	Instream in;
	in.byteStream = new ByteStream(16);
	in.write(Bytecode::ldc, 0, 4);
	in.write(0, 4);
	in.write(Bytecode::ldc, 0, 4);
	in.write(1, 4);
	in.write(Bytecode::add);
	in.write(Bytecode::ldc, 0, 4);
	in.write(1, 4);
	in.write(Bytecode::ldc, 0, 4);
	in.write(2, 4);
	in.write(Bytecode::mul);
	in.write(Bytecode::ldc, 0, 4);
	in.write(2, 4);
	in.write(Bytecode::ldc, 0, 4);
	in.write(3, 4);
	in.write(Bytecode::div);
	in.write(Bytecode::end);
	for (isize i = 0; i < in.byteStream->getUsedSize(); ++i) {
		printf("%d ", in.byteStream->get(i));
	}
	printf("\n%u\n", in.byteStream->reg(2));
	for (isize i = 0; in.byteStream->get(i) != (byte) Bytecode::end; i += in.advLen) {
		printf("%s\n", ~in.dump(i));
	}
	Stack<byte> baseStack(1);
	Stack<byte> stack(1);
	for (isize i = 0; i < 66; ++i) { baseStack.push(i); }
	printf("bstCapacity:%u/%u\n", baseStack.usedSize(), baseStack.capacity());
	stack.set(baseStack.sp, baseStack.up);
	stack.push(6);
	printf("stack.bp : %d baseStack.sp : %d\n", *stack.bp, *baseStack.sp);
	for (isize i = 0; baseStack.usedSize() != 0; ++i) {
		printf("%4u : %-4d\n", i, baseStack.pop());
	}
}

void testCompiler() {
	auto vm = new VM();
	CompileUnit cu(R"(E:\projects\is\script\test.is)", vm);
	isize t = GetTickCount();
	cu.compile();
	t = GetTickCount() - t;
	printf("TimeConsuming:%ums\n", t);
	Instream in(cu.instream.byteStream);
	for (isize i = 0; in.byteStream->get(i) != (byte) Bytecode::end; i += in.advLen) {
		printf("%s\n", ~in.dump(i));
	}
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
	printf("a:%p\tb:%p\tinit:%p\n", ~a, ~b, strInit);
	printf("a:%p\tb:%p\tinit:%p\n", ~a, ~b, strInit);
	printf("a:%p\tb:%p\tinit:%p\n", ~a, ~b, strInit);
	printf("a:%p\tb:%p\tinit:%p\n", ~a, ~b, strInit);
	printf("a:%p\tb:%p\tinit:%p\n", ~a, ~b, strInit);
	printf("a:%p\tb:%p\tinit:%p\n", ~a, ~b, strInit);
	printf("a:%p\tb:%p\tinit:%p\n", ~a, ~b, strInit);
	printf("a:%p\tb:%p\tinit:%p\n", ~a, ~b, strInit);
	printf("a:%p\tb:%p\tinit:%p\n", ~a, ~b, strInit);
	printf("a:%p\tb:%p\tinit:%p\n", ~a, ~b, strInit);
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

	for (isize i = 0; i < vary.getUsedSize(); ++i) {
		printf("vary [%d]:%d\n", i, vary[i]);
	}
	printf("varyCapacity:%d/%d\n", vary.getUsedSize(), vary.getCapacity());
	vary[666] = 0;
	printf("A statement after index out of range.\n");
	vary[666] = 0;
	isize idx = vary.reg(121);
	printf("reg(121):%u,vary[%u]:%u\n", idx, idx, vary[idx]);
	for (isize i = 0; i < vary.getUsedSize(); ++i) {
		printf("vary [%d]:%d\n", i, vary[i]);
	}

	char *array = new char[10];
	memcpy(array, "qwq,awa", 8);
	printf("\n%s\n", array);
}



