#pragma once

#include <string>

#include <lexer/token.h>

struct compilation_error;

struct compilation_advice_info {
	std::string code;
	std::string message;
};

// TODO: may rename to hint or suggestion
struct compilation_advice {
	compilation_error* parent;
	compilation_advice_info info;
};