#pragma once

#include <string>
#include <memory>

#include <ast/ast_fwd.h>
#include <ast/ast_node.h>
#include <ast/ast_visitor.h>
#include <representations/types/types_fwd.h>
#include <utils/iclonable.h>

class enum_option : public ast_node, public std::enable_shared_from_this<enum_option> {
public:
	ENABLE_ACCEPT_AST(enum_option)
	ENABLE_CLONE(enum_option, ast_node)

public:
	enum_option() = default;
	enum_option(const std::string& identifier)
		: identifier(identifier) {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + "Enum Option: " + identifier + "\n";
	}

	type_ptr type() const {
		if (declaration) {
			return declaration->type();
		}
		return data_type::UNKNOWN;
	}
	std::string name() const {
		return identifier;
	}

	std::string identifier;
	enum_declaration* declaration;

protected:
	virtual void clone_into(ast_node& target) const override {
		ast_node::clone_into(target);
		enum_option& option_target = ast_cast<enum_option&>(target);
		option_target.identifier = identifier;
		option_target.declaration = declaration;
	}
};