#pragma once

#include <memory>
#include <string>

#include <ast_passes/classes/computed_value.h>
#include <ast/ast_visitor.h>
#include <ast/ast_fwd.h>
#include <representations/types/data_type.h>
#include <error/internal.h>

class compile_time_computator_visitor : ast_visitor {
	compile_time_computator_visitor()
		: result(data_type::UNKNOWN, "") {
	}

	//void visit(function_declaration& func) override;
	//void visit(variable_declaration& var) override;
	//void visit(type_declaration& decl);
	//void visit(module_declaration& mod);
	//void visit(conversion_declaration& conv);
	//void visit(operator_declaration& op);
	//void visit(enum_declaration& enum_decl);
	//void visit(enum_option& option);
	//void visit(expression_statement& expr);
	void visit(binary_expression& expr);
	//void visit(assignment_expression& expr);
	//void visit(member_expression& expr);
	//void visit(address_of_expression& expr);
	//void visit(deref_expression& expr);
	//void visit(unary_expression& expr);
	//void visit(index_expression& expr);
	//void visit(identifier_expression& id) override;
	//void visit(this_expression& expr);
	//void visit(cast_expression& expr);
	//void visit(initializer_list& init);
	//void visit(function_call& func_call);
	void visit(literal& literal) override;
	//void visit(import_statement& import_stmt);
	//void visit(code_block& block) override;
	//void visit(if_statement& if_stmt) override;
	//void visit(inline_if& inline_if);
	//void visit(for_loop& for_loop);
	//void visit(while_loop& while_loop);
	//void visit(return_statement& ret_stmt) override;
	//void visit(break_statement& brk_stmt);

private:
	computed_value result;

	template<typename Nty>
	computed_value accept(Nty* node) {
		s_assert(node != nullptr, "Cannot compute value of null node");

		result = computed_value(data_type::UNKNOWN, "");
		node->accept(*this);
		return result;
	}

	void not_constant_expr() const;
};