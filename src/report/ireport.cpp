#include "ireport.h"
#include "../lexer/ilexer.h"
#include "../compiler/icompiler.h"
#include <cstdio>
#include <cstdarg>
#include <cstdlib>

RepIdInfo RepIdInfoList[] = {
		#define loadEnum(n, field, level, fmt) \
        {#n, RepField::field, RepLevel::level, fmt},
		#include "repId.enum"
		#undef loadEnum
};

void reportMsg(RepId id, void *ptr, ...) {
	RepIdInfo idInfo = repIdInfo(id);   //获取该RepId的其他信息
	char outbuffer[REPORT_BUFFER_SIZE] = {0};
	char tmpbuffer[REPORT_BUFFER_SIZE] = {0};
	va_list args;
	va_start(args, ptr);//设置args为ptr之后的参数
	vsnprintf(outbuffer, REPORT_BUFFER_SIZE, idInfo.repFmt, args);
	va_end(args);//清除可变参数
	const char *format = nullptr;
	switch (idInfo.level) {  //对其严重等级进行处理
		case RepLevel::unk:
			unreachableBranch();
			return;
		case RepLevel::info:
			format = "\033[0m\033[1;32minfo\033[0m: %s";
			break;
		case RepLevel::warning:
			format = "\033[0m\033[1;33mwarning\033[0m: %s";
			break;
		case RepLevel::error:
			format = "\033[0m\033[1;31merror\033[0m: %s";
			break;
		case RepLevel::fatal:
			format = "\033[0m\033[1;31mfatal\033[0m: %s";
			break;
	}
	sprintf(tmpbuffer, format, outbuffer);
	switch (idInfo.field) {  //对其所属领域进行处理
		case RepField::unk:
			unreachableBranch();
			return;
		case RepField::core:
			sprintf(outbuffer, "core %s", tmpbuffer);
			break;
		case RepField::lexer: { //lexer的情况下需要输出文件名与行列号
			if (ptr == nullptr) { unreachableBranch(); }
			auto *lexer = (Lexer *) ptr;
			sprintf(outbuffer, "%s:%d:%d: %s", ~lexer->fileName,
			        lexer->pos.line, lexer->pos.column, tmpbuffer);
			break;
		}
		case RepField::compiler: {
			if (ptr == nullptr) { unreachableBranch(); }
			auto *cu = (CompileUnit *) ptr;
			cu->errCurFile = cu->errCurStmt = true;
			sprintf(outbuffer, "%s:%d:%d: %s", ~cu->lexer.fileName,
			        cu->lexer.curToken.pos.line, cu->lexer.curToken.pos.column, tmpbuffer);
			break;
		}
		case RepField::vm:
			sprintf(outbuffer, "runtime %s", tmpbuffer);
			break;
	}
	printf("%s\n", outbuffer);
	if (idInfo.level == RepLevel::fatal) { exit((int) id); }
}
