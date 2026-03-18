#pragma once

#include <string>
#include <memory>
#include <utility>

#include <ast/ast_visitor.h>
#include <ast/expressions/expression.h>
#include <parser/parser_utils.h>
#include <representations/types/types_fwd.h>
#include <utils/iclonable.h>

class initializer_list : public expression, public std::enable_shared_from_this<initializer_list> {
public:
	ENABLE_ACCEPT_AST(initializer_list)
	ENABLE_CLONE(initializer_list, ast_node)

public:
	initializer_list() = default;
	initializer_list(std::vector<std::unique_ptr<expression>> args)
		: values(std::move(args)) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Initializer List:\n";
		result += ind + " Values:\n";
		if (values.size() <= 0) {
			result += ind + "  <None>\n";
		}
		else {
			for (int i = 0; i < values.size(); i++) {
				result += values[i]->string(indent + 1) + "\n";
			}
		}
		return result;
	}

	type_ptr type() const override {
		return result_type ? result_type : data_type::UNKNOWN;
	}
	bool is_rvalue() const override {
		return true; // initializer list always return a temporary value
		// really, we are never going to check this as this can only be used
		// on the right hand of an assignment
	}
	bool is_constant() const override {
		for (const auto& val : values) {
			if (!val->is_constant()) {
				return false;
			}
		}
		return true;
	}

	std::vector<std::unique_ptr<expression>> values;
	type_ptr result_type;
	bool is_array_initializer = false;

protected:
	virtual void clone_into(ast_node& target) const override {
		expression::clone_into(target);
		initializer_list& init_target = ast_cast<initializer_list&>(target);
		init_target.values.clear();
		for (const auto& val : values) {
			init_target.values.push_back(ast_cast<expression>(val->clone()));
		}
		init_target.result_type = result_type;
		init_target.is_array_initializer = is_array_initializer;
	}
};