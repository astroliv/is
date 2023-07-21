#include "itoken.h"
#include <cstdio>
#include <cstring>

TokenKindInfo TokenKindInfoList[] = {
#define loadEnum(n, symbol) {#n, symbol},
#include "tokenKind.enum"
#undef loadEnum
};

TokenKind isKeyword(stringMeta extract) {
	for (auto i = (isize) TokenKind::_ + 1; i < (isize) TokenKind::__; ++i) {
		if (!strncmp(tokenKindInfo((TokenKind) i).symbol, extract.getData(), extract.getLength())) {
			return (TokenKind) i;
		}
	}
	return TokenKind::unk;//不是关键词诶qwq
}

string Token::dump() const {
	char buffer[512] = {0};
	if (kind == TokenKind::real) {
		sprintf(buffer, "kind:%-10s extract:%-8s value:%-18f pos:[%u,%u]",
		        tokenKindInfo(kind).strName, ~string(extract),
		        value.f64, pos.line, pos.column);
	} else if (kind == TokenKind::str) {
		sprintf(buffer, "kind:%-10s extract:%-8s value:%-18s pos:[%u,%u]",
		        tokenKindInfo(kind).strName, ~string(extract),
		        ~string(*value.fsp), pos.line, pos.column);
	} else {
		sprintf(buffer, "kind:%-10s extract:%-8s value:%-18s pos:[%u,%u]",
		        tokenKindInfo(kind).strName, ~string(extract),
		        "null", pos.line, pos.column);
	}
	return string(buffer);
}
