#pragma once

#include <string>
#include <memory>
#include <utility>

#include <ast/ast_visitor.h>
#include <ast/expressions/expression.h>
#include <utils/iclonable.h>

class assignment_expression : public expression, public std::enable_shared_from_this<assignment_expression> {
public:
	ENABLE_ACCEPT_AST(assignment_expression)
	ENABLE_CLONE(assignment_expression, ast_node)

public:
	assignment_expression() = default;
	assignment_expression(std::unique_ptr<expression> left, std::unique_ptr<expression> right)
		: left(std::move(left)), right(std::move(right)) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Assignment Expression:\n";
		result += ind + " Assignee:\n" + left->string(indent + 1) + "\n";
		result += ind + " Value:\n" + right->string(indent + 1) + "\n";
		return result;
	}

	type_ptr type() const override {
		return left->type();
	}
	bool is_rvalue() const override {
		return true;
	}
	bool is_constant() const override {
		return false;
	}

	std::unique_ptr<expression> left;
	std::unique_ptr<expression> right;

protected:
	virtual void clone_into(ast_node& target) const override {
		expression::clone_into(target);
		assignment_expression& assign_target = ast_cast<assignment_expression&>(target);
		assign_target.left = ast_cast<expression>(left->clone());
		assign_target.right = ast_cast<expression>(right->clone());
	}
};