#pragma once

#include <string>
#include <map>
#include <unordered_set>

#include <ast/ast_visitor.h>
#include <ast/ast_pass.h>
#include <symbolics/symbol_table.h>

class flow_analyzer : public ast_pass {
public:
	flow_analyzer(compilation_ctx& ctx)
		: ast_pass(ctx) {
	}

	void visit(function_declaration& func) override;
	void visit(code_block& block) override;
	void visit(return_statement& ret_stmt) override;
	void visit(break_statement& brk_stmt) override;
	void visit(if_statement& if_stmt) override;
	void visit(for_loop& for_loop) override;
	void visit(while_loop& while_loop) override;

private:
	std::shared_ptr<function_declaration> current_function = nullptr;
	std::shared_ptr<function_declaration> current_constructor = nullptr;
	bool in_loop = false;

	bool current_conditionally_returns = false;
	bool current_returns = false;
};