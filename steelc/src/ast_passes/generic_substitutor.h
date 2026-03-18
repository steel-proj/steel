#pragma once

#include <string>
#include <map>
#include <vector>

#include <ast/ast_visitor.h>
#include <ast/ast_fwd.h>
#include <representations/types/types_fwd.h>

struct compilation_ctx;

class generic_substitutor : public ast_visitor {
public:
	generic_substitutor(std::vector<type_ptr> substitution_list)
		: substitution_list(substitution_list) {
	}

	void visit(function_declaration& func) override;
	void visit(variable_declaration& var) override;
	void visit(type_declaration& decl) override;
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
	std::vector<type_ptr> substitution_list;

	// this function is much more safe for getting
	// substitutions as it WONT throw if index is OOB
	type_ptr get_substitution(size_t parameter_index) const;
	void try_substitute(type_ptr& type_ref) const;
};