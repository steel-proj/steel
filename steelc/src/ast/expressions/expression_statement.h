#pragma once

#include <string>
#include <memory>
#include <utility>

#include <ast/ast_node.h>
#include <ast/ast_visitor.h>
#include <ast/expressions/expression.h>
#include <utils/iclonable.h>

class expression_statement : public ast_node, public std::enable_shared_from_this<expression_statement> {
public:
	ENABLE_ACCEPT_AST(expression_statement)
	ENABLE_CLONE(expression_statement, ast_node)

public:
	expression_statement() = default;
	expression_statement(std::unique_ptr<expression> expr)
		: expr(std::move(expr)) {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + "Expression Statement:\n" + expr->string(indent + 1);
	}

	std::unique_ptr<expression> expr;

protected:
	virtual void clone_into(ast_node& target) const override {
		ast_node::clone_into(target);
		expression_statement& stmt_target = ast_cast<expression_statement&>(target);
		stmt_target.expr = ast_cast<expression>(expr->clone());
	}
};