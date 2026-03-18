#pragma once

#include <string>
#include <map>
#include <memory>
#include <unordered_set>

#include <ast/ast_visitor.h>
#include <ast/ast_pass.h>
#include <compiler/compilation_ctx.h>
#include <compiler/passes/ir_pass.h>
#include <modules/module_manager.h>
#include <symbolics/symbol_table.h>
#include <representations/entities/module_entity.h>

class declaration_collector : public ast_pass {
public:
	declaration_collector(compilation_ctx& ctx)
		: ast_pass(ctx), module_manager(ctx.module_manager), sym_table(&ctx.module_manager.get_global_module()->symbols()) {
		current_module = module_manager.get_global_module();
	}

	void visit(compilation_unit& unit) override;

	// declarations
	void visit(function_declaration& func_decl) override;
	void visit(variable_declaration& var_decl) override;
	void visit(type_declaration& type_decl) override;
	void visit(module_declaration& mod_decl) override;
	void visit(enum_declaration& enum_decl) override;

	// top-level statements
	void visit(import_statement& import_stmt) override;

private:
	symbol_table* sym_table;
	module_manager& module_manager;

	compilation_unit* current_unit = nullptr;
	std::shared_ptr<function_declaration> current_function = nullptr;
	std::shared_ptr<function_declaration> current_constructor = nullptr;
	std::shared_ptr<type_declaration> current_type = nullptr;
	module_entity* current_module = nullptr; // should never be null - at least the global module
};