#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <unordered_map>

#include <ast/ast_visitor.h>
#include <ast/ast_fwd.h>
#include <ast/ast_pass.h>
#include <compiler/compilation_ctx.h>
#include <modules/module_manager.h>

class import_resolver : public ast_pass {
public:
	import_resolver(compilation_ctx& ctx)
		: ast_pass(ctx), module_manager(ctx.module_manager) {
	}

	void visit(compilation_unit& unit) override;

	void visit(import_statement& import_stmt) override;

private:
	compilation_unit* current_unit = nullptr;
	module_manager& module_manager;
};