#pragma once

#include <string>
#include <memory>
#include <vector>

#include <ast/ast_node.h>
#include <ast/ast_visitor.h>
#include <utils/iclonable.h>

class attribute : public ast_node, public std::enable_shared_from_this<attribute> {
public:
	ENABLE_ACCEPT_AST(attribute)
	ENABLE_CLONE(attribute, ast_node)

public:
	attribute() = default;
	attribute(std::string name)
		: name(name) {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + "Attribute: '" + name + "'";
	}

	std::string name;

protected:
	virtual void clone_into(ast_node& target) const override {
		ast_node::clone_into(target);
		attribute& attr_target = ast_cast<attribute&>(target);
		attr_target.name = name;
	}
};