#pragma once

#include <ast/ast_visitor.h>
#include <ast/compilation_unit.h>
#include <compiler/passes/ir_pass.h>

struct compilation_ctx;

class ast_pass : public ast_visitor, public ir_pass<compilation_unit> {
public:
	ast_pass(compilation_ctx& ctx)
		: ir_pass<compilation_unit>(ctx) {
	}

	void run(compilation_unit& unit) final override {
		unit.accept(*this);
	}
};
