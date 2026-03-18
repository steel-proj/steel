#pragma once

#include <string>
#include <memory>

#include <ast/ast_visitor.h>
#include <ast/expressions/expression.h>
#include <utils/iclonable.h>

class literal : public expression, public std::enable_shared_from_this<literal> {
public:
	ENABLE_ACCEPT_AST(literal)
	ENABLE_CLONE(literal, ast_node)

public:
	literal() = default;
	literal(data_type_kind primitive, std::string value)
		: primitive(primitive), value(value) {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + value;
	}

	// accept is removed as this is a base class only,
	// in the future i may consider making this the only literal class
	// as all derived classes only use string values anyway

	virtual type_ptr type() const override {
		return data_type::get(primitive);
	}
	virtual bool is_rvalue() const override {
		return true; // literals are always rvalues
	}
	virtual bool is_constant() const override {
		return true; // literals are always constants
	}

	data_type_kind primitive;
	std::string value;

protected:
	virtual void clone_into(ast_node& target) const override {
		expression::clone_into(target);
		literal& lit_target = ast_cast<literal&>(target);
		lit_target.primitive = primitive;
		lit_target.value = value;
	}
};