#include <cstdio>
#include <cstring>
#include <windows.h>
#include "container/iarray.h"
#include "container/istack.h"
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
	testLexer();
	testCompiler();
//	testCompound();
	return 0;
}

void testCompound() {
	Stack<byte> baseStack(1);
	Stack<byte> stack(1);
	for (isize i = 0; i < 66; ++i) { baseStack.push(i); }
	printf("bstCapacity:%u/%u\n", baseStack.usedSize(), baseStack.capacity());
	stack.set(baseStack.sp, baseStack.up);
	stack.push(6);
	printf("stack.sb : %d baseStack.sp : %d\n", *stack.bp, *baseStack.sp);
	for (isize i = 0; baseStack.usedSize() != 0; ++i) {
		printf("%4u : %-4d\n", i, baseStack.pop());
	}
}

void testCompiler() {
	delete[] (int *) nullptr;
	auto vm = new VM();
	Compiler com(vm);
	CompileUnit &cu = com.cuList[0];
	cu.init(R"(E:\projects\is\script\test.is)", &com);
	isize t = GetTickCount();
	cu.compile();
	t = GetTickCount() - t;
	printf("TimeConsuming : %ums\n", t);
	printf("Bytes :");
	for (isize i = 0; i < com.instream.getUsedSize(); ++i) { printf(" %d", com.instream[i]); }
	printf("\n");
	VMPRegist(vm, ib).ip8 = VMPRegist(vm, ip).ip8 = com.instream.getPtr(0);
	while (*VMPRegist(vm, ip).ip8 != (byte) Bytecode::end
	       && *VMPRegist(vm, ip).ip8 != (byte) Bytecode::unk) {
		printf("%s\n", ~vm->dumpin());
	}
	printf("Const List:\n");
	for (isize i = 0; i < vm->constList.getUsedSize(); ++i) { printf("%f\n", vm->constList[i].f64); }
	VMPRegist(vm, ib).ip8 = VMPRegist(vm, ip).ip8 = com.instream.getPtr(0);
	vm->execute();
	printf("\nCalcResult:%f\n", vm->pop().f64);
	double f64 = 114514;
	printf("%f\n", -f64 + f64);
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
	printf("regist(121):%u,vary[%u]:%u\n", idx, idx, vary[idx]);
	for (isize i = 0; i < vary.getUsedSize(); ++i) {
		printf("vary [%d]:%d\n", i, vary[i]);
	}

	char *array = new char[10];
	memcpy(array, "qwq,awa", 8);
	printf("\n%s\n", array);
}



