#pragma once

#include <string>
#include <memory>
#include <utility>

#include <ast/ast_visitor.h>
#include <ast/expressions/expression.h>
#include <representations/types/type_utils.h>
#include <utils/iclonable.h>

class address_of_expression : public expression, public std::enable_shared_from_this<address_of_expression> {
public:
	ENABLE_ACCEPT_AST(address_of_expression)
	ENABLE_CLONE(address_of_expression, ast_node)

public:
	address_of_expression() = default;
	address_of_expression(std::unique_ptr<expression> value)
		: value(std::move(value)) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Address Of Expression:\n";
		result += ind + " Value:\n" + value->string(indent + 1) + "\n";
		return result;
	}

	type_ptr type() const override {
		return make_pointer(value->type());
	}
	bool is_rvalue() const override {
		return true;
	}
	bool is_constant() const override {
		return false;
	}

	std::unique_ptr<expression> value;

protected:
	virtual void clone_into(ast_node& target) const override {
		expression::clone_into(target);
		address_of_expression& addr_target = ast_cast<address_of_expression&>(target);
		addr_target.value = ast_cast<expression>(value->clone());
	}
};