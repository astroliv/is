#include "itoken.h"

const char *tokenKindName[] = {
#define HANDLE_ENUM(name, sName) sName,
#include "enum/tokenKind.enum"
#undef HANDLE_ENUM
};
