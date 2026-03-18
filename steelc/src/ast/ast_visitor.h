#pragma once

#include <map>
#include <string>
#include <memory>

#include <ast/ast_fwd.h>

class ast_visitor {
public:
	virtual void visit(compilation_unit& program);
	virtual void visit(function_declaration& func);
	virtual void visit(variable_declaration& var);
	virtual void visit(type_declaration& decl);
	virtual void visit(module_declaration& mod);
	virtual void visit(conversion_declaration& conv);
	virtual void visit(operator_declaration& op);
	virtual void visit(enum_declaration& enum_decl);
	virtual void visit(enum_option& option);
	virtual void visit(expression_statement& expr);
	virtual void visit(binary_expression& expr);
	virtual void visit(assignment_expression& expr);
	virtual void visit(member_expression& expr);
	virtual void visit(address_of_expression& expr);
	virtual void visit(deref_expression& expr);
	virtual void visit(unary_expression& expr);
	virtual void visit(index_expression& expr);
	virtual void visit(identifier_expression& expr);
	virtual void visit(this_expression& expr);
	virtual void visit(cast_expression& expr);
	virtual void visit(initializer_list& init);
	virtual void visit(function_call& func_call);
	virtual void visit(literal& literal);
	virtual void visit(import_statement& import_stmt);
	virtual void visit(code_block& block);
	virtual void visit(if_statement& if_stmt);
	virtual void visit(inline_if& inline_if);
	virtual void visit(for_loop& for_loop);
	virtual void visit(while_loop& while_loop);
	virtual void visit(return_statement& ret_stmt);
	virtual void visit(break_statement& brk_stmt);
	virtual void visit(generic_parameter& param);
	virtual void visit(attribute& attrib);
};