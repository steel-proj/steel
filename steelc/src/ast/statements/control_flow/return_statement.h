#pragma once

#include <string>
#include <utility>

#include <ast/ast_node.h>
#include <ast/ast_visitor.h>
#include <ast/expressions/expression.h>
#include <utils/iclonable.h>

class return_statement : public ast_node, public std::enable_shared_from_this<return_statement> {
public:
	ENABLE_ACCEPT_AST(return_statement)
	ENABLE_CLONE(return_statement, ast_node)

public:
	return_statement() = default;
	return_statement(std::unique_ptr<expression> value)
		: value(std::move(value)) {
	}
	return_statement(std::unique_ptr<expression> value, std::unique_ptr<expression> condition)
		: value(std::move(value)), condition(std::move(condition)) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Return Statement:\n";
		result += ind + " Return Value:\n";
		if (returns_value()) {
			result += value->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <None>\n";
		}
		return result;
	}

	inline bool is_conditional() const {
		return condition != nullptr;
	}
	inline bool returns_value() const {
		return value != nullptr;
	}

	std::unique_ptr<expression> value = nullptr;
	std::unique_ptr<expression> condition = nullptr;

protected:
	virtual void clone_into(ast_node& target) const override {
		ast_node::clone_into(target);
		return_statement& return_target = ast_cast<return_statement&>(target);
		if (value) {
			return_target.value = ast_cast<expression>(value->clone());
		}
		if (condition) {
			return_target.condition = ast_cast<expression>(condition->clone());
		}
	}
};