#pragma once

#include <string>
#include <vector>
#include <utility>

#include <ast/ast_fwd.h>
#include <ast/ast_utils.h>
#include <ast/ast_visitor.h>
#include <ast/declarations/declaration.h>
#include <ast/declarations/variable_declaration.h>
#include <ast/generics/generic_parameter.h>
#include <representations/types/types_fwd.h>
#include <representations/types/function_type.h>
#include <representations/entities/entity_ref.h>
#include <utils/iclonable.h>
#include <error/internal.h>

class function_declaration : public declaration, public std::enable_shared_from_this<function_declaration> {
public:
	ENABLE_ACCEPT_AST(function_declaration)
	ENABLE_CLONE(function_declaration, ast_node)

public:
	function_declaration() = default;
	function_declaration(type_ptr return_type, std::string identifier, std::vector<std::unique_ptr<variable_declaration>> parameters)
		: return_type(return_type), identifier(identifier), parameters(std::move(parameters)), body(nullptr) /* for built in functions */ {
	}
	function_declaration(type_ptr return_type, std::string identifier, std::vector<std::unique_ptr<variable_declaration>> parameters, std::unique_ptr<ast_node> body, bool is_override)
		: return_type(return_type), identifier(identifier), parameters(std::move(parameters)), body(std::move(body)), is_override(is_override) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Function Declaration: \"" + identifier + "\"\n";
		result += ind + " Type: " + return_type->name() + "\n";
		result += ind + " Parameters:\n";
		if (parameters.empty()) {
			result += ind + "  <None>\n";
		}
		else {
			for (int i = 0; i < parameters.size(); i++) {
				const auto& param = parameters[i];
				result += ind + "  - " + param->type->name() + " " + param->identifier + "\n";
			}
		}
		if (body) {
			result += ind + " Body:\n";
			result += body->string(indent + 1);
		}
		else {
			result += ind + " Body: Not defined\n";
		}
		return result;
	}

	type_ptr type() const {
		return function_type::make(return_type, get_param_types());
	}

	inline bool is_abstract() const {
		return body == nullptr;
	}

	inline std::vector<type_ptr> get_param_types() const {
		std::vector<type_ptr> expected_types;
		for (const auto& param : parameters) {
			expected_types.push_back(param->type);
		}
		return expected_types;
	}

	type_ptr return_type;
	std::string identifier;
	std::vector<std::unique_ptr<generic_parameter>> generics;
	std::vector<std::unique_ptr<variable_declaration>> parameters;
	std::unique_ptr<ast_node> body;
	bool is_method = false;
	bool is_generic = false;
	bool is_override = false;
	bool is_constructor = false;
	bool is_generic_instance = false;
	bool is_entry_point = false;
	bool implicitly_returns = false;
	bool no_mangle = false;
	std::vector<type_ptr> generic_args; // if applicable
	type_declaration* parent_type = nullptr;
	function_declaration* overridden_function = nullptr;
	entity_ref func_ref;

protected:
	virtual void clone_into(ast_node& target) const override {
		declaration::clone_into(target);
		function_declaration& func_target = ast_cast<function_declaration&>(target);
		func_target.return_type = return_type;
		func_target.identifier = identifier;
		for (const auto& gen : generics) {
			func_target.generics.emplace_back(
				ast_cast<generic_parameter>(gen->clone()));
		}
		for (const auto& param : parameters) {
			func_target.parameters.emplace_back(
				ast_cast<variable_declaration>(param->clone()));
		}
		if (body) {
			func_target.body = body->clone();
		}
		func_target.is_method = is_method;
		func_target.is_generic = is_generic;
		func_target.is_override = is_override;
		func_target.is_constructor = is_constructor;
		func_target.is_generic_instance = is_generic_instance;
		func_target.is_entry_point = is_entry_point;
		func_target.implicitly_returns = implicitly_returns;
		func_target.no_mangle = no_mangle;
		func_target.generic_args = generic_args;
	}
};