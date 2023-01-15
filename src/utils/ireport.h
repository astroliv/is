//报告系统的实现(包括四类报告: 信息.Info, 警告.Warning, 错误.Error, 致命.fatal)
#ifndef IS_IREPORT_H
#define IS_IREPORT_H
#include <cstdint>
#include "options.h"

//不应到达的分支
#ifdef DEBUG_MODE
#define unreachableBranch() \
reportMsg(RepId::unreachableBranch, nullptr, __FILE__, __LINE__)
#else
#define unreachableBranch();
#endif

//报告所属领域
enum class RepField : uint8_t { unk, core, lexer, compiler, vm };
//报告严重等级
enum class RepLevel : uint8_t { unk, info, waring, error, fatal };
//报告标识符
enum class RepId : uint16_t {
	// repId strRepId belongLevel belongField repFmtText
	#define HANDLE_ENUM(n, sn, field, level, fmt) n,
	#include "../enum/repId.enum"
	#undef HANDLE_ENUM
};

struct RepIdMsg {
	const char *strRepId{nullptr};
	RepField field{RepField::unk};
	RepLevel level{RepLevel::unk};
	const char *repFmt{nullptr};
};

extern RepIdMsg repIdMsg[];

void reportMsg(RepId id, void *ptr, ...);

#endif //IS_IREPORT_H
