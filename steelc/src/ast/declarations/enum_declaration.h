#pragma once

#include <string>
#include <memory>
#include <vector>
#include <utility>

#include <ast/ast_fwd.h>
#include <ast/ast_visitor.h>
#include <ast/declarations/declaration.h>
#include <ast/declarations/variable_declaration.h>
#include <ast/declarations/function_declaration.h>
#include <ast/declarations/operator_declaration.h>
#include <representations/types/enum_type.h>
#include <utils/iclonable.h>

class enum_declaration : public declaration, public std::enable_shared_from_this<enum_declaration> {
public:
	ENABLE_ACCEPT_AST(enum_declaration)
	ENABLE_CLONE(enum_declaration, ast_node)

public:
	enum_declaration() = default;
	explicit enum_declaration(const std::string& identifier)
		: identifier(identifier) {
	}
	enum_declaration(const std::string& identifier, std::vector<std::unique_ptr<enum_option>> options)
		: identifier(identifier), options(std::move(options)) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Enum Declaration:\n";
		result += ind + " Identifier: " + identifier + "\n";
		result += ind + " Options:\n";
		if (options.empty()) {
			result += ind + "  <None>\n";
		}
		else {
			
		}
		return result;
	}

	type_ptr type() {
		if (!cached_type) {
			cached_type = std::make_shared<enum_type>(identifier, shared_from_this());
		}
		return cached_type;
	}
	std::string name() const {
		return identifier;
	}

	std::string identifier;
	type_ptr base_type;
	std::vector<std::unique_ptr<enum_option>> options;

protected:
	virtual void clone_into(ast_node& target) const override {
		declaration::clone_into(target);
		enum_declaration& enum_target = ast_cast<enum_declaration&>(target);
		enum_target.identifier = identifier;
		enum_target.base_type = base_type;
		for (const auto& option : options) {
			enum_target.options.emplace_back(
				ast_cast<enum_option>(option->clone()));
		}
	}

private:
	type_ptr cached_type;
};
