#pragma once

#include <string>
#include <vector>

#include <ast/ast_node.h>
#include <ast/ast_visitor.h>
#include <utils/iclonable.h>

class compilation_unit;

class import_statement : public ast_node, public std::enable_shared_from_this<import_statement> {
public:
	ENABLE_ACCEPT_AST(import_statement)
	DISABLE_CLONE(import_statement, ast_node)

public:
	import_statement() = default;
	import_statement(std::vector<std::string> module_path)
		: module_path(module_path) {
	}

	// TODO: USE NAME_PATH!!
	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Import Statement: ";
		for (size_t i = 0; i < module_path.size(); ++i) {
			result += module_path[i];
			if (i < module_path.size() - 1) {
				result += "::";
			}
		}
		return result;
	}

	std::vector<std::string> module_path;
};