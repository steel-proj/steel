#pragma once

#include <string>
#include <memory>
#include <utility>

#include <ast/ast_node.h>
#include <ast/ast_visitor.h>
#include <ast/expressions/expression.h>
#include <utils/iclonable.h>

class while_loop : public ast_node, public std::enable_shared_from_this<while_loop> {
public:
	ENABLE_ACCEPT_AST(while_loop)
	ENABLE_CLONE(while_loop, ast_node)

public:
	while_loop() = default;
	while_loop(std::unique_ptr<expression> condition, std::unique_ptr<ast_node> body)
		: condition(std::move(condition)), body(std::move(body)) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "While loop:\n";
		result += ind + " Condition:\n";
		if (condition) {
			result += condition->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <Empty>\n";
		}
		result += ind + " Body:\n";
		if (body) {
			result += body->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <Empty>\n";
		}
		return result;
	}

	std::unique_ptr<expression> condition;
	std::unique_ptr<ast_node> body;

protected:
	virtual void clone_into(ast_node& target) const override {
		ast_node::clone_into(target);
		while_loop& loop_target = ast_cast<while_loop&>(target);
		if (condition) {
			loop_target.condition = ast_cast<expression>(condition->clone());
		}
		if (body) {
			loop_target.body = body->clone();
		}
	}
};