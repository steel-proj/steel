#pragma once

#include <lexer/token.h>

enum modifier_kind {
	MOD_NONE = 0,
	MOD_EXTERN = 1 << 0,
	MOD_STATIC = 1 << 1,
	MOD_EXPORT = 1 << 2,
	MOD_CONST = 1 << 3,
};

typedef unsigned int modifier_flags;

struct modifier {
	modifier_kind kind = MOD_NONE;
	token tk; // (for diagnostics)
};