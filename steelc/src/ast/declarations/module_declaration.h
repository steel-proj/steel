#pragma once

#include <string>
#include <memory>
#include <utility>

#include <ast/ast_fwd.h>
#include <ast/ast_visitor.h>
#include <ast/declarations/declaration.h>
#include <representations/entities/entities_fwd.h>
#include <utils/iclonable.h>

class module_declaration : public declaration, public std::enable_shared_from_this<module_declaration> {
public:
	ENABLE_ACCEPT_AST(module_declaration)
	DISABLE_CLONE(module_declaration, ast_node)

public:
	module_declaration() = default;
	module_declaration(std::string name, std::vector<std::unique_ptr<ast_node>> declarations)
		: name(name), declarations(std::move(declarations)), entity(nullptr) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Module Declaration: \"" + name + "\"\n";
		return result;
	}

	std::string name;
	std::vector<std::unique_ptr<ast_node>> declarations;
	module_entity* entity;
};