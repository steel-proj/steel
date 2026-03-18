#pragma once

#include <string>
#include <memory>
#include <vector>

#include <lexer/token.h> // for code span - make this its own header?

class compilation_step;
class source_file;

enum class compilation_diagnostic_kind {
	ERROR,
	WARNING,
	HINT
};

struct compilation_diagnostic_info {
	std::string code;
	std::string message;
};

struct compilation_diagnostic {
	compilation_diagnostic_kind kind;
	compilation_diagnostic_info info;
	code_span span;
	compilation_step* step = nullptr;
	source_file* source = nullptr;
	std::vector<compilation_diagnostic> hints = {};
};