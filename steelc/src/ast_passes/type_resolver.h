#pragma once

#include <string>
#include <unordered_map>

#include <ast/ast_visitor.h>
#include <ast/ast_fwd.h>
#include <ast/ast_pass.h>
#include <ast/compilation_unit.h>
#include <compiler/compilation_ctx.h>
#include <representations/entities/entities_fwd.h>
#include <representations/entities/module_entity.h>
#include <symbolics/import_table.h>
#include <symbolics/symbol_resolver.h>
#include <symbolics/symbol_table.h>
#include <modules/module_manager.h>

class type_resolver : public ast_pass {
public:
	type_resolver(compilation_ctx& ctx)
		: ast_pass(ctx), module_manager(ctx.module_manager) {
	}

	void visit(compilation_unit& unit) override;

	void visit(function_declaration& func) override;
	void visit(variable_declaration& var) override;
	void visit(type_declaration& decl) override;
	void visit(module_declaration& decl) override;
	void visit(function_call& func_call) override;

private:
	symbol_resolver resolver;
	module_manager& module_manager;
	symbol_table* sym_table = nullptr;
	unsigned int cur_generic_index = 0;

	void resolve_type(type_ptr& type);
	void resolve_custom(type_ptr& custom);
};