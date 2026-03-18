#pragma once

#include <memory>

#include <ast/ast_visitor.h>
#include <symbolics/symbol_table.h>

class entity_refresher : public ast_visitor {
public:
	entity_refresher(const symbol_table& sym_table)
		: sym_table(sym_table) {
	}

	void visit(identifier_expression& expr) override;
	void visit(member_expression& expr) override;

private:
	const symbol_table& sym_table;
};