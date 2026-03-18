#pragma once

#include <string>
#include <memory>

#include <ast/ast_visitor.h>
#include <ast/expressions/expression.h>
#include <representations/types/types_fwd.h>
#include <utils/iclonable.h>

class this_expression : public expression, public std::enable_shared_from_this<this_expression> {
public:
	ENABLE_ACCEPT_AST(this_expression)
	ENABLE_CLONE(this_expression, ast_node)

public:
	this_expression() = default;

	std::string string(int indent) const override {
		return indent_s(indent) + "This Expression";
	}

	type_ptr type() const override {
		return parent_type;
	}
	bool is_rvalue() const override {
		return false;
	}
	bool is_constant() const override {
		return false;
	}

	type_ptr parent_type;

protected:
	virtual void clone_into(ast_node& target) const override {
		expression::clone_into(target);
		this_expression& this_target = ast_cast<this_expression&>(target);
		this_target.parent_type = parent_type;
	}
};