#pragma once

#include <string>
#include <memory>
#include <utility>

#include <ast/ast_node.h>
#include <ast/ast_visitor.h>
#include <ast/expressions/expression.h>
#include <lexer/token_type.h>
#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>
#include <representations/entities/entities_fwd.h>
#include <representations/entities/entity.h>
#include <representations/entities/entity_ref.h>
#include <representations/entities/variable_entity.h>
#include <utils/iclonable.h>

class member_expression : public expression, public std::enable_shared_from_this<member_expression> {
public:
	ENABLE_ACCEPT_AST(member_expression)
	ENABLE_CLONE(member_expression, ast_node)

public:
	member_expression() = default;
	member_expression(std::unique_ptr<expression> object, std::string member, token_type access_operator)
		: object(std::move(object)), member(member), access_operator(access_operator) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Member Expression:\n";
		result += ind + " Left Identifier:\n";
		if (object) {
			result += object->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <None>\n";
		}
		result += ind + " Member Identifier:\n";
		result += ind + "  \"" + member + "\"\n";
		return result;
	}

	virtual type_ptr type() const override {
		return resolved_type ? resolved_type : data_type::UNKNOWN;
	}
	bool is_rvalue() const override {
		return !is_lvalue;
	}
	bool is_constant() const override {
		return false;
	}

	entity_ptr entity(const symbol_table& sym_table) override {
		return entity_ref.resolve(sym_table);
	}
	entity_ptr entity() override {
		return entity_ref.get();
	}

	inline bool is_resolved() const {
		return resolved_type != nullptr && entity_ref.get() != entity::UNRESOLVED;
	}

	inline bool is_static_access() const {
		return access_operator == TT_SCOPE;
	}
	inline bool is_instance_access() const {
		return access_operator == TT_ACCESS || access_operator == TT_ARROW;
	}

	std::unique_ptr<expression> object;
	std::string member;
	token_type access_operator;
	type_ptr resolved_type;
	entity_ref entity_ref;
	bool is_lvalue = true;

protected:
	virtual void clone_into(ast_node& target) const override {
		expression::clone_into(target);
		member_expression& mem_target = ast_cast<member_expression&>(target);
		if (object) {
			mem_target.object = ast_cast<expression>(object->clone());
		}
		else {
			mem_target.object = nullptr;
		}
		mem_target.member = member;
		mem_target.access_operator = access_operator;
		mem_target.resolved_type = resolved_type;
		mem_target.entity_ref = entity_ref;
		mem_target.is_lvalue = is_lvalue;
	}
};