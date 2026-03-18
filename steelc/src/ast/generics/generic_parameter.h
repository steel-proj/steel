#pragma once

#include <string>
#include <memory>

#include <ast/ast_node.h>
#include <ast/ast_visitor.h>
#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>
#include <utils/iclonable.h>

enum generic_param_type {
	GENERIC_TYPE,
	GENERIC_VALUE,
};

class generic_parameter : public ast_node, public std::enable_shared_from_this<generic_parameter> {
public:
	ENABLE_ACCEPT_AST(generic_parameter)
	ENABLE_CLONE(generic_parameter, ast_node)
	
public:
	generic_parameter() = default;
	generic_parameter(const std::string& identifier, generic_param_type type = GENERIC_TYPE)
		: identifier(identifier), type(type), param_index(0), substitution(data_type::UNKNOWN) {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + "Generic Parameter: '" + identifier + "'";
	}

	std::string identifier;
	generic_param_type type;
	int param_index;
	// (doesnt actually do anything, purely for type rememering)
	type_ptr substitution;

protected:
	virtual void clone_into(ast_node& target) const override {
		ast_node::clone_into(target);
		generic_parameter& param_target = ast_cast<generic_parameter&>(target);
		param_target.identifier = identifier;
		param_target.type = type;
		param_target.param_index = param_index;
		param_target.substitution = substitution;
	}
};