#pragma once

#include <string>
#include <memory>
#include <utility>

#include <ast/ast_node.h>
#include <ast/ast_visitor.h>
#include <ast/expressions/expression.h>
#include <utils/iclonable.h>

class for_loop : public ast_node, public std::enable_shared_from_this<for_loop> {
public:
	ENABLE_ACCEPT_AST(for_loop)
	ENABLE_CLONE(for_loop, ast_node)

public:
	for_loop() = default;
	for_loop(std::unique_ptr<ast_node> initializer, std::unique_ptr<expression> condition, std::unique_ptr<ast_node> increment, std::unique_ptr<ast_node> body)
		: initializer(std::move(initializer)), condition(std::move(condition)), increment(std::move(increment)), body(std::move(body)) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "For loop:\n";
		result += ind + " Initializer:\n";
		if (initializer) {
			result += initializer->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <None>\n";
		}
		result += ind + " Condition:\n";
		if (condition) {
			result += condition->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <None>\n";
		}
		result += ind + " Increment:\n";
		if (increment) {
			result += increment->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <None>\n";
		}
		result += ind + " Body:\n";
		if (body) {
			result += body->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <None>\n";
		}
		return result;
	}

	std::unique_ptr<ast_node> initializer;
	std::unique_ptr<expression> condition;
	std::unique_ptr<ast_node> increment;
	std::unique_ptr<ast_node> body;

protected:
	virtual void clone_into(ast_node& target) const override {
		ast_node::clone_into(target);
		for_loop& loop_target = ast_cast<for_loop&>(target);
		if (initializer) {
			loop_target.initializer = initializer->clone();
		}
		if (condition) {
			loop_target.condition = ast_cast<expression>(condition->clone());
		}
		if (increment) {
			loop_target.increment = increment->clone();
		}
		if (body) {
			loop_target.body = body->clone();
		}
	}
};