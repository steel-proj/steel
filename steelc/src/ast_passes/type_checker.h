#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <unordered_map>

#include <ast/ast_visitor.h>
#include <ast/ast_fwd.h>
#include <ast/ast_pass.h>
#include <compiler/compilation_ctx.h>
#include <symbolics/symbol_table.h>
#include <modules/module_manager.h>
#include <representations/entities/module_entity.h>

class type_checker : public ast_pass {
public:
	type_checker(compilation_ctx& ctx)
		: ast_pass(ctx) {
		active_symbols = &ctx.module_manager.get_global_module()->symbols();
	}

	void visit(function_declaration& func) override;
	void visit(variable_declaration& var) override;
	void visit(type_declaration& decl) override;
	void visit(module_declaration& module) override;
	void visit(identifier_expression& expr) override;
	void visit(binary_expression& expr) override;
	void visit(assignment_expression& expr) override;
	void visit(address_of_expression& expr) override;
	void visit(deref_expression& expr) override;
	void visit(unary_expression& expr) override;
	void visit(index_expression& expr) override;
	void visit(cast_expression& expr) override;
	void visit(member_expression& expr) override;
	void visit(initializer_list& init) override;
	void visit(function_call& func_call) override;
	void visit(if_statement& if_stmt) override;
	void visit(inline_if& inline_if) override;
	void visit(for_loop& for_loop) override;
	void visit(while_loop& while_loop) override;
	void visit(return_statement& ret_stmt) override;

private:
	const symbol_table* active_symbols;
	std::shared_ptr<function_declaration> current_function = nullptr;

	std::unordered_map<std::shared_ptr<function_declaration>, std::map<std::vector<type_ptr>, std::shared_ptr<function_declaration>>> generic_function_instances;
	std::unordered_map<std::shared_ptr<type_declaration>, std::map<std::vector<type_ptr>, std::shared_ptr<type_declaration>>> generic_type_instances;
	std::vector<std::vector<type_ptr>> generic_substitution_stack;

	struct candidate_score {
		std::shared_ptr<function_declaration> candidate;
		int score = 0;
	};

	type_ptr resolve_expr_type(expression* expr);	

	bool member_access_allowed(type_ptr type);
	bool method_access_allowed(type_ptr type);
	bool is_valid_conversion(type_ptr from, type_ptr to, bool implicit, code_span span);
	bool is_valid_upcast(type_ptr from, type_ptr to);
	bool is_valid_entry_point(std::shared_ptr<function_declaration> entry);

	int score_candidate(std::shared_ptr<function_declaration> candidate, const std::vector<type_ptr>& arg_types);

	void check_type(type_ptr& type);
	void unbox_type(type_ptr& type);
	type_ptr auto_deref(type_ptr type);

	std::shared_ptr<function_declaration> unbox_generic_func(std::shared_ptr<function_declaration> generic_func, const std::vector<type_ptr>& types);
	std::shared_ptr<type_declaration> unbox_generic_type(std::shared_ptr<type_declaration> generic_type, const std::vector<type_ptr>& types);
};