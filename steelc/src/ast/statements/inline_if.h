#pragma once

#include <string>
#include <memory>
#include <utility>

#include <ast/ast_node.h>
#include <ast/ast_visitor.h>
#include <ast/expressions/expression.h>
#include <utils/iclonable.h>

class inline_if : public ast_node, public std::enable_shared_from_this<inline_if> {
public:
	ENABLE_ACCEPT_AST(inline_if)
	ENABLE_CLONE(inline_if, ast_node)

public:
	inline_if() = default;
	inline_if(std::unique_ptr<expression> condition, std::unique_ptr<expression> statement)
		: condition(std::move(condition)), statement(std::move(statement)) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "If Statement:\n";
		result += ind + " Condition:\n";
		if (condition) {
			result += condition->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <Empty>\n";
		}
		result += ind + " Statement:\n";
		if (statement) {
			result += statement->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <Empty>\n";
		}
		return result;
	}

	std::unique_ptr<expression> condition;
	std::unique_ptr<expression> statement;

protected:
	virtual void clone_into(ast_node& target) const override {
		ast_node::clone_into(target);
		inline_if& if_target = ast_cast<inline_if&>(target);
		if (condition) {
			if_target.condition = ast_cast<expression>(condition->clone());
		}
		if (statement) {
			if_target.statement = ast_cast<expression>(statement->clone());
		}
	}
};