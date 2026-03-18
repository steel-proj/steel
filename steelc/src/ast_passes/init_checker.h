#pragma once

#include <string>
#include <map>
#include <unordered_set>

#include <ast/ast_visitor.h>
#include <ast/ast_fwd.h>
#include <ast/ast_pass.h>
#include <symbolics/symbol_table.h>

class init_checker : public ast_pass {
public:
	init_checker(compilation_ctx& ctx)
		: ast_pass(ctx) {
	}

	void visit(type_declaration& decl) override;
	void visit(function_declaration& func_decl) override;
	void visit(variable_declaration& var) override;
	void visit(assignment_expression& expr) override;
	void visit(identifier_expression& expr) override;
	void visit(code_block& block) override;
	void visit(if_statement& if_stmt) override;
	void visit(for_loop& for_loop) override;
	void visit(while_loop& while_loop) override;

private:
	std::unordered_set<std::shared_ptr<variable_declaration>> initialized;
	bool in_method = false;

	void traverse_block(std::shared_ptr<code_block>& block, bool restore);
	bool default_initialized(type_ptr type);
};