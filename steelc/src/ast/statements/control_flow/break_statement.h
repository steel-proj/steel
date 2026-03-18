#pragma once

#include <string>
#include <utility>

#include <ast/ast_node.h>
#include <ast/ast_visitor.h>
#include <ast/expressions/expression.h>
#include <utils/iclonable.h>

class break_statement : public ast_node, public std::enable_shared_from_this<break_statement> {
public:
	ENABLE_ACCEPT_AST(break_statement)
	ENABLE_CLONE(break_statement, ast_node)

public:
	break_statement() = default;
	break_statement(std::unique_ptr<expression> condition)
		: condition(std::move(condition)) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Break Statement:\n";
		return result;
	}

	inline bool is_conditional() const {
		return condition != nullptr;
	}

	std::unique_ptr<expression> condition = nullptr;

protected:
	virtual void clone_into(ast_node& target) const override {
		ast_node::clone_into(target);
		break_statement& break_target = ast_cast<break_statement>(target);
		if (condition) {
			break_target.condition = ast_cast<expression>(condition->clone());
		}
	}
};