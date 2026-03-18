#pragma once

#include <string>
#include <memory>
#include <utility>

#include <ast/ast_visitor.h>
#include <ast/expressions/expression.h>
#include <utils/iclonable.h>

class cast_expression : public expression, public std::enable_shared_from_this<cast_expression> {
public:
	ENABLE_ACCEPT_AST(cast_expression)
	ENABLE_CLONE(cast_expression, ast_node)

public:
	cast_expression() = default;
	cast_expression(type_ptr cast_type, std::unique_ptr<expression> expr)
		: cast_type(cast_type), expr(std::move(expr)) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Cast Expression:\n";
		result += ind + " Cast Type: " + cast_type->name() + "\n";
		result += ind + " Expression: " + expr->string(indent + 1) + "\n";
		return result;
	}

	type_ptr type() const override {
		if (!cast_type) {
			return data_type::UNKNOWN;
		}
		return cast_type;
	}
	bool is_rvalue() const override {
		return false;
	}
	bool is_constant() const override {
		return expr->is_constant();
	}

	type_ptr cast_type;
	std::unique_ptr<expression> expr;

protected:
	virtual void clone_into(ast_node& target) const override {
		expression::clone_into(target);
		cast_expression& cast_target = ast_cast<cast_expression&>(target);
		cast_target.cast_type = cast_type;
		cast_target.expr = ast_cast<expression>(expr->clone());
	}
};