#include "ast_visitor.h"

#include <ast/ast.h>

void ast_visitor::visit(compilation_unit& program) {
	for (const auto& decl : program.declarations) {
		decl->accept(*this);
	}
}
void ast_visitor::visit(function_declaration& func) {
	for (const auto& gen : func.generics) {
		gen->accept(*this);
	}
	for (const auto& param : func.parameters) {
		param->accept(*this);
	}
	if (func.body) {
		func.body->accept(*this);
	}
}
void ast_visitor::visit(variable_declaration& var) {
	if (var.has_initializer()) {
		var.initializer->accept(*this);
	}
}
void ast_visitor::visit(type_declaration& decl) {
	for (const auto& constructor : decl.constructors) {
		constructor->accept(*this);
	}
	for (const auto& member : decl.fields) {
		member->accept(*this);
	}
	for (const auto& method : decl.methods) {
		method->accept(*this);
	}
	for (const auto& op : decl.operators) {
		op->accept(*this);
	}
}
void ast_visitor::visit(module_declaration& mod) {
	for (const auto& decl : mod.declarations) {
		decl->accept(*this);
	}
}
void ast_visitor::visit(conversion_declaration&) {

}
void ast_visitor::visit(operator_declaration&) {

}
void ast_visitor::visit(enum_declaration& enum_decl) {
	for (const auto& option : enum_decl.options) {
		option->accept(*this);
	}
}
void ast_visitor::visit(enum_option&) {
}
void ast_visitor::visit(expression_statement& expr) {
	expr.expr->accept(*this);
}
void ast_visitor::visit(binary_expression& expr) {
	expr.left->accept(*this);
	expr.right->accept(*this);
}
void ast_visitor::visit(assignment_expression& expr) {
	expr.left->accept(*this);
	expr.right->accept(*this);
}
void ast_visitor::visit(member_expression& expr) {
	expr.object->accept(*this);
}
void ast_visitor::visit(address_of_expression& expr) {
	expr.value->accept(*this);
}
void ast_visitor::visit(deref_expression& expr) {
	expr.value->accept(*this);
}
void ast_visitor::visit(unary_expression& expr) {
	expr.operand->accept(*this);
}
void ast_visitor::visit(index_expression& expr) {
	expr.base->accept(*this);
	expr.indexer->accept(*this);
}
void ast_visitor::visit(identifier_expression&) {
}
void ast_visitor::visit(this_expression&) {
}
void ast_visitor::visit(cast_expression& expr) {
	expr.expr->accept(*this);
}
void ast_visitor::visit(initializer_list& init) {
	for (const auto& value : init.values) {
		value->accept(*this);
	}
}
void ast_visitor::visit(function_call& func_call) {
	// accept callee for methods
	if (func_call.is_method()) {
		func_call.callee->accept(*this);
	}

	// accept all args
	for (auto& arg : func_call.args) {
		arg->accept(*this);
	}
}
void ast_visitor::visit(literal&) {

}
void ast_visitor::visit(import_statement&) {
	
}
void ast_visitor::visit(code_block& block) {
	for (const auto& stmt : block.body) {
		stmt->accept(*this);
	}
}
void ast_visitor::visit(if_statement& if_stmt) {
	if_stmt.condition->accept(*this);
	if_stmt.then_node->accept(*this);
	if (if_stmt.else_node) {
		if_stmt.else_node->accept(*this);
	}
}
void ast_visitor::visit(inline_if& inline_if) {
	inline_if.condition->accept(*this);
	inline_if.statement->accept(*this);
}
void ast_visitor::visit(for_loop& for_loop) {
	if (for_loop.initializer) {
		for_loop.initializer->accept(*this);
	}
	if (for_loop.condition) {
		for_loop.condition->accept(*this);
	}
	if (for_loop.increment) {
		for_loop.increment->accept(*this);
	}
	for_loop.body->accept(*this);
}
void ast_visitor::visit(while_loop& while_loop) {
	while_loop.condition->accept(*this);
	while_loop.body->accept(*this);
}
void ast_visitor::visit(return_statement& ret_stmt) {
	if (ret_stmt.condition) {
		ret_stmt.condition->accept(*this);
	}
	if (ret_stmt.value) {
		ret_stmt.value->accept(*this);
	}
}
void ast_visitor::visit(break_statement& brk_stmt) {
	if (brk_stmt.condition) {
		brk_stmt.condition->accept(*this);
	}
}
void ast_visitor::visit(generic_parameter&) {
}
void ast_visitor::visit(attribute&) {
}
