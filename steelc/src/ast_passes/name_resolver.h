#pragma once

#include <string>
#include <map>
#include <memory>

#include <ast/ast_visitor.h>
#include <ast/compilation_unit.h>
#include <ast/ast_pass.h>
#include <compiler/compilation_ctx.h>
#include <symbolics/symbol_table.h>
#include <symbolics/symbol_resolver.h>
#include <modules/module_manager.h>
#include <representations/entities/entities_fwd.h>
#include <representations/entities/module_entity.h>

class name_resolver : public ast_pass {
public:
	name_resolver(compilation_ctx& ctx)
		: ast_pass(ctx), module_manager(ctx.module_manager) {
	}

	void visit(compilation_unit& unit) override;

	void visit(function_declaration& func) override;
	void visit(variable_declaration& var) override;
	void visit(type_declaration& decl) override;
	void visit(module_declaration& module) override;
	void visit(binary_expression& expr) override;
	void visit(identifier_expression& expr) override;
	void visit(member_expression& expr) override;
	void visit(this_expression& expr) override;
	void visit(function_call& func_call) override;
	void visit(code_block& block) override;
	void visit(if_statement& if_stmt) override;
	void visit(for_loop& for_loop) override;
	void visit(while_loop& while_loop) override;

private:
	symbol_resolver resolver;
	module_manager& module_manager;
	symbol_table* sym_table;

	std::shared_ptr<function_declaration> current_func;
	std::shared_ptr<function_declaration> current_ctor;
	std::shared_ptr<type_declaration> current_type;

	// helper that returns nullptr if no current type
	std::shared_ptr<type_entity> current_type_entity() const;

	void resolve_type_names(type_ptr& type);
};