#ifndef IS_ITOKEN_H
#define IS_ITOKEN_H

#include "../basic/ibasic.h"

struct Position {
	isize line{1};
	isize column{1};
};

enum class TokenKind : uint8_t {
	#define loadEnum(n, symbol) n,
	#include "tokenKind.enum"
	#undef loadEnum
};

struct Token {
	refString extract;              //对应的文本
	TokenKind kind{TokenKind::unk}; //类型
	Value value;                    //解析值
	Position pos;                   //位置

	string dump() const;            //输出信息
};

struct TokenKindInfo {
	const char *strName{};
	const char *symbol{};
};

extern TokenKindInfo TokenKindInfoList[];

inline TokenKindInfo tokenKindInfo(TokenKind tk) {
	return TokenKindInfoList[(isize) tk];
}

TokenKind isKeyword(stringMeta extract);

#endif //IS_ITOKEN_H
