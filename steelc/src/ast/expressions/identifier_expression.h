#pragma once

#include <string>
#include <memory>

#include <ast/ast_visitor.h>
#include <ast/expressions/expression.h>
#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>
#include <representations/entities/entities_fwd.h>
#include <representations/entities/entity.h>
#include <representations/entities/entity_ref.h>
#include <representations/entities/variable_entity.h>
#include <symbolics/symbol_table.h>
#include <utils/iclonable.h>

struct module_info;

class identifier_expression : public expression, public std::enable_shared_from_this<identifier_expression> {
public:
	ENABLE_ACCEPT_AST(identifier_expression)
	ENABLE_CLONE(identifier_expression, ast_node)

public:
	identifier_expression() = default;
	identifier_expression(std::string identifier)
		: identifier(identifier) {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + "Identifier \"" + identifier + "\"";
	}

	std::unique_ptr<ast_node> clone() const override {
		auto cloned = std::make_unique<identifier_expression>(identifier);
		return cloned;
	}

	type_ptr type() const override {
		if (auto ent = entity_ref.get(); ent && ent != entity::UNRESOLVED) {
			if (ent->kind() == ENTITY_VARIABLE) {
				return ent->as_variable()->var_type();
			}
		}
		return data_type::UNKNOWN;
	}
	bool is_rvalue() const override {
		// technically yes if variable, but we dont have that
		// kind of context here, maybe we will change this later
		return false;
	}
	bool is_constant() const override {
		// TODO: we could allow for CONSTANT variables here
		return false;
	}

	entity_ptr entity(const symbol_table& sym_table) override {
		return entity_ref.resolve(sym_table);
	}
	entity_ptr entity() override {
		return entity_ref.get();
	}

	std::string identifier;
	entity_ref entity_ref;

protected:
	virtual void clone_into(ast_node& target) const override {
		expression::clone_into(target);
		identifier_expression& id_target = ast_cast<identifier_expression&>(target);
		id_target.identifier = identifier;
		id_target.entity_ref = entity_ref;
	}
};