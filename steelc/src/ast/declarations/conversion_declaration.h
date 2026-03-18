#pragma once

#include <memory>
#include <utility>
#include <string>
#include <vector>

#include <ast/ast_fwd.h>
#include <ast/ast_visitor.h>
#include <ast/declarations/declaration.h>
#include <ast/declarations/variable_declaration.h>
#include <parser/parser_utils.h>
#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>
#include <utils/iclonable.h>
#include <utils/disable_copy.h>

class conversion_declaration : public declaration, public std::enable_shared_from_this<conversion_declaration> {
public:
	ENABLE_ACCEPT_AST(conversion_declaration)
	DISABLE_CLONE(conversion_declaration, ast_node)

public:
	conversion_declaration() = default;
	conversion_declaration(type_ptr from, type_ptr to, std::unique_ptr<ast_node> body)
		: from(from), to(to), body(std::move(body)), implicit(false) {
	}
	conversion_declaration(type_ptr from, type_ptr to, std::unique_ptr<ast_node> body, bool implicit)
		: from(from), to(to), body(std::move(body)), implicit(implicit) {
	}
	conversion_declaration(type_ptr from, type_ptr to, bool implicit)
		: from(from), to(to), body(nullptr), implicit(implicit) {
	}
	conversion_declaration(data_type_kind from, data_type_kind to, bool implicit = false)
		: from(to_data_type(from)), to(to_data_type(to)), body(nullptr), implicit(implicit) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Conversion Declaration:\n";
		result += ind + " From Type: " + from->name() + "\n";
		result += ind + " To Type: " + to->name() + "\n";
		result += ind + " Implicit: " + std::string(implicit ? "true" : "false") + "\n";
		if (body) {
			result += ind + " Body:\n";
			result += body->string(indent + 1);
		}
		else {
			result += ind + " Body: Not defined\n";
		}
		return result;
	}

	type_ptr from;
	type_ptr to;
	std::unique_ptr<ast_node> body;
	bool implicit;
};