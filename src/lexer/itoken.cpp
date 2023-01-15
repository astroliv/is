#include "itoken.h"
#include <cstdio>

const char *tokenKindName[] = {
#define HANDLE_ENUM(n, sn) sn,
#include "../enum/tokenKind.enum"
#undef HANDLE_ENUM
};

string Token::dump() const {
	char buffer[512] = {0};
	if (kind == TokenKind::num) {
		sprintf(buffer, "kind:%-10s extract:%-8s value:%-18f pos:[%u,%u]",
		        tokenKindName[(uint8_t) kind], ~string(extract),
		        value.f64, pos.line, pos.column);
	} else if (kind == TokenKind::str) {
		sprintf(buffer, "kind:%-10s extract:%-8s value:%-18s pos:[%u,%u]",
		        tokenKindName[(uint8_t) kind], ~string(extract),
		        ~string(*value.fstr), pos.line, pos.column);
	} else {
		sprintf(buffer, "kind:%-10s extract:%-8s value:%-18s pos:[%u,%u]",
		        tokenKindName[(uint8_t) kind], ~string(extract),
		        "null", pos.line, pos.column);
	}
	return string(buffer);
}
