#ifndef IS_ITOKEN_H
#define IS_ITOKEN_H

#include "../basic/ibasic.h"

struct Position {
	uint32_t line{1};
	uint32_t column{1};
};

enum class TokenKind : uint8_t {
	#define loadEnum(n) n,
	#include "tokenKind.enum"
	#undef loadEnum
};

extern const char *tokenKindName[];

struct Token {
	refString extract;              //对应的文本
	TokenKind kind{TokenKind::unk}; //类型
	Value value{0};                 //对应的文本的值
	Position pos;                   //位置

	string dump() const;            //输出信息
};


#endif //IS_ITOKEN_H
