#pragma once

#include <string>
#include <memory>
#include <utility>

#include <ast/ast_visitor.h>
#include <ast/declarations/declaration.h>
#include <ast/expressions/expression.h>
#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>
#include <representations/entities/entity_ref.h>
#include <utils/iclonable.h>

class variable_declaration : public declaration, public std::enable_shared_from_this<variable_declaration> {
public:
	ENABLE_ACCEPT_AST(variable_declaration)
	ENABLE_CLONE(variable_declaration, ast_node)

public:
	variable_declaration() = default;
	variable_declaration(bool is_const, type_ptr type, std::string identifier)
		: is_const(is_const), type(type), identifier(identifier), initializer(nullptr) {
	}
	variable_declaration(bool is_const, type_ptr type, std::string identifier, std::unique_ptr<expression> initializer)
		: is_const(is_const), type(type), identifier(identifier), initializer(std::move(initializer)), initialized(initializer) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Variable Declaration:\n";
		result += ind + " Type: " + type->name() + "\n";
		result += ind + " Identifier: " + identifier + "\n";
		result += ind + " Initializer:\n";
		if (initializer) {
			result += initializer->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <None>\n";
		}
		return result;
	}

	inline bool has_initializer() const {
		return initializer != nullptr;
	}

	type_ptr type;
	std::string identifier;
	std::unique_ptr<expression> initializer;
	bool initialized = false;
	bool is_const = false;
	bool is_parameter = false;
	bool is_field = false;
	size_t field_index = 0;
	entity_ref var_ref;

protected:
	virtual void clone_into(ast_node& target) const override {
		declaration::clone_into(target);
		variable_declaration& var_target = ast_cast<variable_declaration&>(target);
		var_target.is_const = is_const;
		var_target.type = type;
		var_target.identifier = identifier;
		if (initializer) {
			var_target.initializer = ast_cast<expression>(initializer->clone());
		}
		var_target.initialized = initialized;
		var_target.is_parameter = is_parameter;
		var_target.is_field = is_field;
		var_target.field_index = field_index;
		var_target.var_ref = var_ref;
	}
};