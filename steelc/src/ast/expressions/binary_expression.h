#pragma once

#include <string>
#include <memory>
#include <utility>

#include <ast/ast_visitor.h>
#include <ast/expressions/expression.h>
#include <lexer/token_type.h>
#include <lexer/token_utils.h>
#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>
#include <utils/iclonable.h>

class binary_expression : public expression, public std::enable_shared_from_this<binary_expression> {
public:
	ENABLE_ACCEPT_AST(binary_expression)
	ENABLE_CLONE(binary_expression, ast_node)

public:
	binary_expression() = default;
	binary_expression(std::unique_ptr<expression> left, std::unique_ptr<expression> right, token_type oparater)
		: left(std::move(left)), right(std::move(right)), oparator(oparater) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Binary Expression:\n";
		result += ind + " Operator: " + to_string(oparator) + "\n";
		result += ind + " Left:\n" + left->string(indent + 1) + "\n";
		result += ind + " Right:\n" + right->string(indent + 1) + "\n";
		return result;
	}

	type_ptr type() const override {
		if (!result_type) {
			return data_type::UNKNOWN;
		}
		return result_type;
	}
	bool is_rvalue() const override {
		return true;
	}
	bool is_constant() const override {
		return left->is_constant() && right->is_constant();
	}

	std::unique_ptr<expression> left;
	std::unique_ptr<expression> right;
	token_type oparator;
    type_ptr result_type;

protected:
	virtual void clone_into(ast_node& target) const override {
		expression::clone_into(target);
		binary_expression& bin_target = ast_cast<binary_expression&>(target);
		bin_target.left = ast_cast<expression>(left->clone());
		bin_target.right = ast_cast<expression>(right->clone());
		bin_target.oparator = oparator;
		bin_target.result_type = result_type;
	}
};