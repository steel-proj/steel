#pragma once

#include <string>
#include <memory>
#include <utility>

#include <ast/ast_visitor.h>
#include <ast/expressions/expression.h>
#include <lexer/token_type.h>
#include <lexer/token_utils.h>
#include <representations/types/types_fwd.h>
#include <utils/iclonable.h>

class unary_expression : public expression, public std::enable_shared_from_this<unary_expression> {
public:
	ENABLE_ACCEPT_AST(unary_expression)
	ENABLE_CLONE(unary_expression, ast_node)

public:
	unary_expression() = default;
	unary_expression(token_type op, std::unique_ptr<expression> operand)
		: oparator(op), operand(std::move(operand)) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Binary Expression:\n";
		result += ind + " Operand:\n" + operand->string(indent + 1) + "\n";
		result += ind + " Operator: " + to_string(oparator) + "\n";
		return result;
	}

	type_ptr type() const override {
		return operand->type();
	}
	bool is_rvalue() const override {
		return false;
	}
	bool is_constant() const override {
		return operand->is_constant();
	}

	token_type oparator;
	std::unique_ptr<expression> operand;

protected:
	virtual void clone_into(ast_node& target) const override {
		expression::clone_into(target);
		unary_expression& unary_target = ast_cast<unary_expression&>(target);
		unary_target.oparator = oparator;
		unary_target.operand = ast_cast<expression>(operand->clone());
	}
};