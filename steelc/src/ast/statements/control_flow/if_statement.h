#pragma once

#include <string>
#include <memory>
#include <utility>

#include <ast/ast_node.h>
#include <ast/ast_visitor.h>
#include <ast/expressions/expression.h>
#include <utils/iclonable.h>

class if_statement : public ast_node, public std::enable_shared_from_this<if_statement> {
public:
	ENABLE_ACCEPT_AST(if_statement)
	ENABLE_CLONE(if_statement, ast_node)

public:
	if_statement() = default;
	if_statement(std::unique_ptr<expression> condition, std::unique_ptr<ast_node> then_node)
		: condition(std::move(condition)), then_node(std::move(then_node)) {
	}
	if_statement(std::unique_ptr<expression> condition, std::unique_ptr<ast_node> then_node, std::unique_ptr<ast_node> else_node)
		: condition(std::move(condition)), then_node(std::move(then_node)), else_node(std::move(else_node)) {
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
		result += ind + " Then Block:\n";
		if (then_node) {
			result += then_node->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <Empty>\n";
		}
		result += ind + " Else Block:\n";
		if (else_node) {
			result += else_node->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <Empty>\n";
		}
		return result;
	}

	// boolean conditional expression
	std::unique_ptr<expression> condition = nullptr;
	// block to execute if condition is true (may be code_block, single statement, etc)
	std::unique_ptr<ast_node> then_node = nullptr;
	// block to execute if condition is false (may be code_block, single statement, if null if no else present)
	std::unique_ptr<ast_node> else_node = nullptr;

protected:
	virtual void clone_into(ast_node& target) const override {
		ast_node::clone_into(target);
		if_statement& if_target = ast_cast<if_statement&>(target);
		if (condition) {
			if_target.condition = ast_cast<expression>(condition->clone());
		}
		if (then_node) {
			if_target.then_node = then_node->clone();
		}
		if (else_node) {
			if_target.else_node = else_node->clone();
		}
	}
};