#pragma once

#include <string>
#include <memory>
#include <utility>

#include <ast/ast_visitor.h>
#include <ast/expressions/expression.h>
#include <lexer/token_type.h>
#include <lexer/token_utils.h>
#include <parser/parser_utils.h>
#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>
#include <representations/types/container_types.h>
#include <utils/iclonable.h>

class index_expression : public expression, public std::enable_shared_from_this<index_expression> {
public:
	ENABLE_ACCEPT_AST(index_expression)
	ENABLE_CLONE(index_expression, ast_node)

public:
	index_expression() = default;
	index_expression(std::unique_ptr<expression> base, std::unique_ptr<expression> indexer)
		: base(std::move(base)), indexer(std::move(indexer)) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Index Expression:\n";
		result += ind + " Base:\n" + base->string(indent + 1) + "\n";
		result += ind + " Indexer:\n" + indexer->string(indent + 1) + "\n";
		return result;
	}

	type_ptr type() const override {
		// TODO: delete whatever this nonsense is
		// all i know is it should be handled elsewhere
		auto t = base->type();
		if (t->is_array()) {
			return std::dynamic_pointer_cast<array_type>(t)->base_type;
		}
		else if (t->is_pointer()) {
			return std::dynamic_pointer_cast<pointer_type>(t)->base_type;
		}
		else if (t->is_primitive() && t->primitive == DT_STRING) {
			return to_data_type(DT_CHAR);
		}
		return base->type();
	}
	bool is_rvalue() const override {
		return false; // usually indexed values are lvalues
		// but i might be wrong
	}
	bool is_constant() const override {
		return false;
	}

	std::unique_ptr<expression> base;
	std::unique_ptr<expression> indexer;

protected:
	virtual void clone_into(ast_node& target) const override {
		expression::clone_into(target);
		index_expression& index_target = ast_cast<index_expression&>(target);
		index_target.base = ast_cast<expression>(base->clone());
		index_target.indexer = ast_cast<expression>(indexer->clone());
	}
};