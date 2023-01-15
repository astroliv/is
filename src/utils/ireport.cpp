#include "ireport.h"
#include "../lexer/ilexer.h"
#include <cstdio>
#include <cstdarg>
#include <cstdlib>

RepIdMsg repIdMsg[] = {
		#define HANDLE_ENUM(n, sn, field, level, fmt) \
        {sn, RepField::field, RepLevel::level, fmt},
		#include "../enum/repId.enum"
		#undef HANDLE_ENUM
};

void reportMsg(RepId id, void *ptr, ...) {
	RepIdMsg idMsg = repIdMsg[(uint16_t) id];   //获取该RepId的其他信息
	char outbuffer[REPORT_BUFFER_SIZE] = {0};
	char tmpbuffer[REPORT_BUFFER_SIZE] = {0};
	va_list args;
	va_start(args, ptr);//设置args为ptr之后的参数
	vsnprintf(outbuffer, REPORT_BUFFER_SIZE, idMsg.repFmt, args);
	va_end(args);//清除可变参数
	const char *format = nullptr;
	switch (idMsg.level) {  //对其严重等级进行处理
		case RepLevel::unk:
			unreachableBranch();
			return;
		case RepLevel::info:
			format = " \033[0m\033[1;32m info\033[0m: %s";
			break;
		case RepLevel::waring:
			format = "\033[0m\033[1;33m warning\033[0m: %s";
			break;
		case RepLevel::error:
			format = "\033[0m\033[1;31m error\033[0m: %s";
			break;
		case RepLevel::fatal:
			format = "\033[0m\033[1;31m fatal\033[0m: %s";
			break;
	}
	sprintf(tmpbuffer, format, outbuffer);
	switch (idMsg.field) {  //对其所属领域进行处理
		case RepField::unk:
			unreachableBranch();
			return;
		case RepField::core:
			sprintf(outbuffer, "core%s", tmpbuffer);
			break;
		case RepField::lexer: { //lexer的情况下需要输出文件名与行列号
			if (ptr == nullptr) {
				unreachableBranch();
				return;
			}
			auto *lexer = (Lexer *) ptr;
			sprintf(outbuffer, "%s:%d:%d:%s", ~lexer->fileName,
			        lexer->pos.line, lexer->pos.column, tmpbuffer);
			break;
		}
		case RepField::compiler:
			sprintf(outbuffer, "compiler%s", tmpbuffer);
			break;
		case RepField::vm:
			sprintf(outbuffer, "vm%s", tmpbuffer);
			break;
	}
	printf("%s\n", outbuffer);
	if (idMsg.level == RepLevel::fatal) { exit((int) id); }
}
