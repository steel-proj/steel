#pragma once

#include <string>
#include <memory>
#include <utility>

#include <ast/ast_visitor.h>
#include <ast/expressions/expression.h>
#include <ast/declarations/function_declaration.h>
#include <ast/declarations/type_declaration.h>
#include <representations/types/types_fwd.h>
#include <utils/iclonable.h>

class function_call : public expression, public std::enable_shared_from_this<function_call> {
public:
	ENABLE_ACCEPT_AST(function_call)
	ENABLE_CLONE(function_call, ast_node)

public:
	function_call() = default;
	function_call(std::string function_name, std::vector<std::unique_ptr<expression>> args)
		: identifier(function_name), args(std::move(args)), declaration(nullptr) {
	}
	function_call(std::unique_ptr<expression> callee, std::string name, std::vector<std::unique_ptr<expression>> args)
		: identifier(name), callee(std::move(callee)), args(std::move(args)), declaration(nullptr) {
	}
	function_call(std::unique_ptr<expression> callee, std::vector<std::unique_ptr<expression>> args)
		: identifier(""), callee(std::move(callee)), args(std::move(args)), declaration(nullptr) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Function Call: \"" + identifier + "\"\n";
		result += ind + " Arguments:\n";
		if (args.size() <= 0) {
			result += ind + "  <None>\n";
		}
		else {
			for (int i = 0; i < args.size(); i++) {
				result += args[i]->string(indent + 1) + "\n";
			}
		}
		return result;
	}

	type_ptr type() const override {
		// returns the RETURN TYPE not the FUNCTION TYPE
		if (!declaration) {
			if (ctor_type) {
				return ctor_type->type();
			}
			return data_type::UNKNOWN;
		}
		return declaration->return_type;
	}
	bool is_rvalue() const override {
		return true; // function calls always return a temporary value
	}
	bool is_constant() const override {
		return false;
	}

	inline bool is_scoped_function() const {
		return scope != nullptr;
		// e.g. module::function()
		// sort of half way between a function and method
	}
	inline bool is_method() const {
		return callee != nullptr;
	}

	std::string identifier;
	std::unique_ptr<expression> scope;
	std::unique_ptr<expression> callee;
	std::vector<std::unique_ptr<expression>> args;
	std::vector<function_declaration*> declaration_candidates;
	function_declaration* declaration;
	std::vector<type_ptr> generic_args;
	type_declaration* ctor_type = nullptr;
	bool is_constructor = false;

protected:
	virtual void clone_into(ast_node& target) const override {
		expression::clone_into(target);
		function_call& call_target = ast_cast<function_call&>(target);
		call_target.identifier = identifier;
		if (scope) {
			call_target.scope = ast_cast<expression>(scope->clone());
		}
		if (callee) {
			call_target.callee = ast_cast<expression>(callee->clone());
		}
		for (const auto& arg : args) {
			call_target.args.emplace_back(
				ast_cast<expression>(arg->clone()));
		}
		call_target.declaration_candidates = declaration_candidates;
		call_target.declaration = declaration;
		call_target.generic_args = generic_args;
		call_target.ctor_type = ctor_type;
		call_target.is_constructor = is_constructor;
	}
};