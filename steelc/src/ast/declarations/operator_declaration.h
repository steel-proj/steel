#pragma once

#include <string>
#include <memory>
#include <utility>

#include <ast/ast_fwd.h>
#include <ast/ast_utils.h>
#include <ast/ast_visitor.h>
#include <ast/declarations/declaration.h>
#include <ast/declarations/operator_declaration.h>
#include <lexer/token_type.h>
#include <parser/parser_utils.h>
#include <representations/types/types_fwd.h>
#include <utils/iclonable.h>

class operator_declaration : public declaration, public std::enable_shared_from_this<operator_declaration> {
public:
	ENABLE_ACCEPT_AST(operator_declaration)
	ENABLE_CLONE(operator_declaration, ast_node)

public:
	operator_declaration() = default;
	operator_declaration(type_ptr result, type_ptr left, token_type oparator, type_ptr right, std::unique_ptr<ast_node> body)
		: result_type(result), left(left), right(right), oparator(oparator), body(std::move(body)) {
	}
	operator_declaration(data_type_kind result, data_type_kind left, token_type oparator, data_type_kind right)
		: result_type(to_data_type(result)), left(to_data_type(left)), right(to_data_type(right)), oparator(oparator), body(nullptr) {
		// used for built in functions that arnt defined in source
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Operator Declaration:\n";
		return result;
	}

	bool matches(const type_ptr& lty, token_type op, const type_ptr& rty) const {
		return *left == lty && oparator == op && *right == rty;
	}

	type_ptr result_type;
	type_ptr left;
	type_ptr right;
	token_type oparator;
	std::unique_ptr<ast_node> body;

protected:
	virtual void clone_into(ast_node& target) const override {
		declaration::clone_into(target);
		operator_declaration& op_target = ast_cast<operator_declaration&>(target);
		op_target.result_type = result_type;
		op_target.left = left;
		op_target.right = right;
		op_target.oparator = oparator;
		if (body) {
			op_target.body = body->clone();
		}
	}
};