#pragma once

#include <string>
#include <memory>
#include <vector>

#include <error/compilation_advice.h>
#include <lexer/token.h>

class compilation_unit;

enum class compilation_error_kind {
	ERROR,
	WARNING
};

struct compilation_error_info {
	std::string code;
	std::string message;
};

struct compilation_error {
	compilation_error_kind kind;
	compilation_error_info info;
	code_span span;
	std::shared_ptr<compilation_unit> unit;
	std::vector<compilation_advice> advices;
};